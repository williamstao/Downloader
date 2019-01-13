#include "multi_downloader.h"
#include "curl_helper.h"
Downloader::Downloader(IMultiDownloaderAdapter* adapter):m_url(""), 
m_start(0),m_end(0),m_file(NULL), m_downloadOver(Dl_Unfinish), m_index(0),m_temp_file(""),m_offset(0),m_chunk_size(0)
{
	m_adapter = adapter;
}

Downloader::~Downloader()
{

}

int Downloader::AddDownloadTask(int start, int end, const std::string& url,int index, const std::string& tmp_file, uint32_t chunk_size)
{
	m_start = start;
	m_end = end;
	m_url = url;
	m_index = index;
	m_temp_file = tmp_file + "." + std::to_string(index);
	m_offset = 0;
	m_chunk_size = chunk_size;
	if (access(m_temp_file.c_str(), 0) == 0)
	{
		uint32_t file_size = 0;
		int err = tp_get_file_size(m_temp_file.c_str(), file_size);
		if (err != 0)
		{
			if (unlink(m_temp_file.c_str()) != 0)
			{
				__ASSERT(0);
				return -1;
			}
		}
		else
		{
			//__DEBUG_LOG("file_size:%d", file_size);
			if (m_start + file_size > m_end)
			{
				//__DEBUG_LOG("write too much");
				if (unlink(m_temp_file.c_str()))
				{
					__ASSERT(0);
					return -1;
				}
			}
			else
			{
				m_start += file_size;
				m_offset = file_size;
			}
		}
	}
	
	Start();
	return 0;
}

void Downloader::DoRun()
{
	DoMultiDownloader();
}

size_t Downloader::curl_download_callback(void* ptr, size_t size, size_t nmemb, void* param)
{
	Downloader* This = (Downloader*)param;
	return This->CurlDownloadCallBack(ptr, size, nmemb);
}

size_t Downloader::CurlDownloadCallBack(void* ptr, size_t size, size_t nmemb)
{
	if (m_adapter == NULL)
	{
		__DEBUG_LOG("m_adapter NULL");
		return 0;
	}

	if (m_adapter->NeedCancel())
	{
		__DEBUG_LOG("need cancel at index:%d", m_index);
		return 0;
	}

	size_t num = size * nmemb;
	if (m_offset + size * nmemb > m_end - m_chunk_size * m_index)
	{
		num = m_end - m_chunk_size * m_index - m_offset;
	}

	size_t ret_value = fwrite(ptr, 1, num, m_file);
	if (ret_value != num)
	{
		__DEBUG_LOG("fwrite error");
		return 0;
	}
	
	if (m_adapter->DownloadPlistFile(m_index, m_offset, num) != 0)
	{
		__DEBUG_LOG("DownloadPlistFile error");
		return 0;
	}
	
	m_offset += num;
	return size * nmemb;
}

void Downloader::StopDownload()
{
	if (m_adapter != NULL)
	{
		m_adapter->SetCancel(true);
	}

	if (m_file != NULL)
	{
		fclose(m_file);
		m_file = NULL;
	}

	m_downloadOver = Dl_Err;
}

void Downloader::DoMultiDownloader()
{
	FILE* f = fopen(m_temp_file.c_str(), "ab+");
	if (f == NULL)
	{
		StopDownload();
		return;
	}
	m_file = f;

	CURL *curl = curl_easy_init();
	if (curl == NULL)
	{
		StopDownload();
		return;
	}

	curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
	curl_setopt_default(curl);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_download_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	if (m_start >= 0 && m_end >= m_start)
	{
		char range[64] = {0};
		sprintf(range, "%d-%d", m_start, m_end);
		__DEBUG_LOG(" %d set range from %d to %d",m_index, m_start, m_end);
		curl_easy_setopt(curl, CURLOPT_RANGE, range);
	}

	CURLcode retval = curl_easy_perform(curl);	
	int retcode = 0;
	if (retval == CURLE_OK)
	{
		retval = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (retval != CURLE_OK)
		{
			retcode = 200;
			__ASSERT(0);
		}
	}
	else
	{
		__DEBUG_LOG("curl_easy_perform, retval:%d", retval);
	}

	curl_easy_cleanup(curl);
	if (retval != CURLE_OK || (retcode != 200 && retcode != 206))
	{
		__DEBUG_LOG("download error");
		StopDownload();
		return;
	}

	if (m_file != NULL)
	{
		fclose(m_file);
		m_file = NULL;
	}
	m_downloadOver = Dl_Ok;
}

int Downloader::IsdownloadOver()
{
	return m_downloadOver;
}
