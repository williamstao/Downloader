#include "curl_helper.h"
#include "http_downloader.h"
#include "multi_downloader.h"
#include "util.h"

#define ChkParmeter(v){ if(v == NULL ||strlen(v)==0) {m_err_code = DlErr_Param;return -1;}}
static int CheckFileCorrect(const char* file_path, const char* expect_file_md5, uint32_t expect_file_size)
{
	if (access(file_path, 0) != 0)
	{
		//__ASSERT(0);
		return -1;
	}

	uint32_t file_size = 0;
	int err = tp_get_file_size(file_path, file_size);
	if (err == 0)
	{
		if (expect_file_size != 0)
		{
			if (file_size != expect_file_size)
			{
				//__ASSERT(0);
				return -1;
			}
		}
	}

	char file_md5[64] = { 0 };
	err = GetFileMd5(file_path, file_md5);
	if (err != 0)
	{
		//__ASSERT(0);
		return -1;
	}

	err = CompareMd5(expect_file_md5, file_md5);
	if (err != 0)
	{
		//__ASSERT(0);
		return -1;
	}

	return 0;
}

BigFileDownloader::BigFileDownloader(IProgressAdapter* adapter) :
	m_total_size(0),
	m_curl(NULL),
	m_is_range_dl(false),
	m_url(""),
	m_tmp_path(""),
	m_except_file_md5(""),
	m_err_code(0),
	m_errno(0),
	m_retval(0),
	m_retcode(0),
	m_from_cache(0),
	m_needcancel(false),
	m_offset(0)
{
	m_adapter = adapter;
}

BigFileDownloader::~BigFileDownloader()
{
	
}

void BigFileDownloader::SetAdapter(IProgressAdapter *adapter)
{
	m_adapter = adapter;
}

void BigFileDownloader::CleanUp(const char *tmp_path)
{
	RangeDlHelper obj;
	obj.CleanRangeDlInfo(tmp_path);
	if (tmp_path != NULL && access(tmp_path, 0) == 0)
	{
		unlink(tmp_path);
	}

	for (int i = 0; i < tp_dlcnt; i++)
	{
		std::string index_tmp_file = std::string(tmp_path) + "." + std::to_string(i);
		if (access(index_tmp_file.c_str(), 0) == 0)
		{
			unlink(index_tmp_file.c_str());
		}
	}
}

void BigFileDownloader::SetCancel(bool isCancel)
{
	m_needcancel = isCancel;
}

bool BigFileDownloader::NeedCancel()
{
	return m_needcancel;
}

size_t BigFileDownloader::curl_download_callback(void* ptr, size_t size, size_t nmemb, void* param)
{
	BigFileDownloader* This = (BigFileDownloader*)param;
	return This->CurlDownloadCallBack(ptr, size, nmemb);
}

size_t BigFileDownloader::CurlDownloadCallBack(void* ptr, size_t size, size_t nmemb)
{
	return size*nmemb;
}

int BigFileDownloader::Download(const char *url, const char* md5, const char *local_path)
{
	ChkParmeter(md5);
	ChkParmeter(url);
	ChkParmeter(local_path);

	int err = -1;
	err = CheckFileCorrect(local_path, md5, 0);
	if (err == 0)
	{
		return 0;
	}

	for (int i = 0; i < 10; i++)
	{
		err = DoDownload(url, md5, local_path);
		if (err == 0)
		{
			break;
		}
	}
	
	return err;
}

//不涉及文件写操作
int BigFileDownloader::GetContentLength(const char* url)
{
	CURL *curl = curl_easy_init();
	if (curl == NULL)
	{
		m_err_code = DLErr_InitCurl;
		m_errno = errno;

		__DEBUG_LOG("curl_easy_init err");
		__ASSERT(0);
		return -1;
	}

	m_curl = curl;
	double length = 0;
	curl_setopt_default(curl);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);    //只需要header头
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);    //不需要body
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_download_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	CURLcode retval = curl_easy_perform(curl);
	if (retval == CURLE_OK)
	{
		int retcode;
		CURLcode retval = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (retval != CURLE_OK)
		{
			retcode = 200;
			__ASSERT(0);
		}
		if (retval != CURLE_OK || (retcode != 200 && retcode != 206))
		{
			length = -1;
		}
		else
		{
			curl_easy_getinfo(m_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &length);
		}
	}
	else
	{
		length = -1;
	}
	curl_easy_cleanup(curl);
	m_total_size = (int)length;
	
	//获得文件大小
	//printf("m_total_size:%d\n", m_total_size);
	if (m_total_size < 0)
	{
		__DEBUG_LOG("Get download range size failed");
		return -1;
	}

	return 0;
}

int BigFileDownloader::DownloadPlistFile(int index, int offset, int num)
{
	TPGuard gurad(m_Lock);
	if (index >= tp_dlcnt || index < 0)
	{
		__ASSERT(0);
		return -1;
	}
	
	if (m_adapter != NULL)
	{
		m_offset += num;
		//__DEBUG_LOG("m_offset: %d", m_offset);
	}

	return 0;
}

int BigFileDownloader::DownloadPlist()
{
	for (int i = 0; i < tp_dlcnt; i++)
	{
		int chunk_size = m_total_size / tp_dlcnt;
		int start = i * chunk_size;
		int end = (i + 1)*(chunk_size);

		if (i == tp_dlcnt - 1)
		{
			end = m_total_size;
		}

		Downloader* dl = new Downloader(this);
		if (dl == NULL)
		{
			__ASSERT(0);
			return -1;
		}
		dLoader.push_back(dl);
		int err = dl->AddDownloadTask(start, end, m_url, i, m_tmp_path, chunk_size);
		if (err != 0)
		{
			return -1;
		}
	}

	bool stop = false;
	bool success = true;
	double start = GetTickCount();
	while (!stop)
	{
		stop = true;
		for (auto it = dLoader.begin(); it != dLoader.end(); it++)
		{
			Downloader* dl = *it;
			if (dl->IsdownloadOver() == Dl_Unfinish)
			{
				stop = false;
				break;
			}
			else if(dl->IsdownloadOver() == Dl_Err)
			{
				success = false;
			}
		}
		Sleep(3000);
		double end = GetTickCount();
		if (end - start > 60 * 60 * 1000)
		{
			break;
		}
	}
	
	//释放下载器
	for (auto it = dLoader.begin(); it != dLoader.end(); it++)
	{
		Downloader* dl = *it;
		if (dl != NULL)
		{
			delete(dl);
			dl = NULL;
		}
	}
	dLoader.clear();

	if (!success)
	{
		__ASSERT(0);
		return -1;
	}

	return 0;
}

int BigFileDownloader::MergeDownloadFile()
{
	std::string temp_file = m_tmp_path;
	if (access(temp_file.c_str(), 0) == 0)
	{
		if (unlink(temp_file.c_str())!=0)
		{
			__ASSERT(0);
			return -1;
		}
	}
	FileClosePool pool;

	FILE* tar_f = fopen(temp_file.c_str(), "ab+");
	if (tar_f == NULL)
	{
		__ASSERT(0);
		return -1;
	}
	pool.Append(tar_f);

	for (int i = 0; i < tp_dlcnt; i++)
	{
		FileClosePool temp_pool;
		std::string temp_file_path = m_tmp_path + "." + std::to_string(i);
		FILE* src_f = fopen(temp_file_path.c_str(), "rb+");
		temp_pool.Append(src_f);

		size_t retval_1, retval_2;
		unsigned char buf[4096] = {0};
		while (1)
		{
			retval_1 = fread(buf, 1, sizeof(buf), src_f);
			if (retval_1 > 0 && retval_1 <= sizeof(buf))
			{
				retval_2 = fwrite(buf, 1, retval_1, tar_f);
				if (retval_2 != retval_1)
				{
					__ASSERT(0);
					return -1;
				}
			}
			if (retval_1 != sizeof(buf) && ferror(src_f))
			{
				__ASSERT(0);
				return -1;
			}
			if (feof(src_f)) break;
		}
	}
	return 0;
}

int BigFileDownloader::DoDownload(const char *url, const char* md5, const char *local_path)
{
	int err = 0;
	m_needcancel = false;

	//组装临时路径
	std::string temp_file = local_path;
	temp_file += ".tmp";
	const char* tmp_file_path = temp_file.c_str();

	err = CheckFileCorrect(tmp_file_path, md5, 0);
	if (err == 0)
	{
		err = rename(tmp_file_path, local_path);
		if (err != 0)
		{
			return -1;
		}
		return 0;
	}

	m_url = url;
	m_tmp_path = tmp_file_path;
	m_except_file_md5 = md5;

	////获得下载长度
	err = GetContentLength(url);
	if (err != 0)
	{
		__ASSERT(0);
		return  -1;
	}

	//设置下载的range-info
	RangeDlHelper helper;
	if (!helper.IsRangeDlInfoCorrect(m_tmp_path.c_str(), m_except_file_md5.c_str(), m_total_size, tp_dlcnt))
	{
		CleanUp(m_tmp_path.c_str());
		RangeDlHelper d_helper;
		int err = d_helper.WriteRangeDlInfo(m_url.c_str(), m_tmp_path.c_str(), m_except_file_md5.c_str(), m_total_size, tp_dlcnt);
		if (err != 0)
		{
			//写失败就用上次的
			__ASSERT(0);
		}
	}
	
	//获得了m_range_download_size
	PerfModule perf("download file");
	perf.start();
	err = DownloadPlist();
	if (err != 0)
	{
		__DEBUG_LOG("download file err");
		return -1;
	}
	
	//合并文件
	err = MergeDownloadFile();
	if(err!=0)
	{
		__ASSERT(0);
		unlink(m_tmp_path.c_str());
		return -1;
	}

	perf.stop("download file");
	__DEBUG_LOG("download all plist finish");

    //下载完后可能是一半的文件，如果删除，断点续传就没意义
	err = CheckFileCorrect(tmp_file_path, md5, m_total_size);
	if (err != 0)
	{
		m_err_code = DLErr_CheckMd5;
		CleanUp(temp_file.c_str());
		__DEBUG_LOG("CheckFileCorrect");
		return -1;
	}

	//在这里重命名的时候互斥操作,重命名时判断文件是否存在，存在就不再下载
	TPGuard guard(m_Lock);
	err = rename(tmp_file_path, local_path);
	if (err != 0)
	{
		m_err_code = DlErr_Rename2;
		m_errno = errno;
		return -1;
	}

	RangeDlHelper d_helper;
	d_helper.CleanRangeDlInfo(tmp_file_path);
	for (int i = 0; i< tp_dlcnt ; i++)
	{
		std::string temp_file_path = m_tmp_path + "." + std::to_string(i);
		if (access(temp_file_path.c_str(), 0) == 0)
		{
			unlink(temp_file_path.c_str());
		}
	}
	return 0;
}

// ============== RangeDlHelper ================
void RangeDlHelper::GetRangeDlInfoFilePath(const char *dl_file_path, char *path, size_t buf_size)
{
	snprintf(path, buf_size, "%s.dl", dl_file_path);
}

int RangeDlHelper::WriteRangeDlInfo(const char *url, const char *dl_file_path,
	const char* file_md5, unsigned int except_file_size, int dl_cnt)
{
	__DEBUG_LOG("WriteRangeDlInfo: %s, %s, %s, %d, %d", url, dl_file_path, file_md5, except_file_size, dl_cnt);
	char path[512] = { 0 };
	char tmp_path[512] = { 0 };
	GetRangeDlInfoFilePath(dl_file_path, path, sizeof(path));
	if (access(path, 0) == 0)
	{
		unlink(path);
	}
	strcpy(tmp_path, path);
	strcat(tmp_path, ".tmp");

	unsigned char *buf = (unsigned char *)malloc(4096);
	if (buf == NULL)
	{
		__ASSERT(0);
		return -1;
	}
	MallocReleasePool pool;
	pool.Append(buf);

	DataStream ds(buf, 4096);
	/*
	magic_code、url、file_md5、file_size、dl_cnt、offsets...
	*/
	ds.write_u32(0x20150202);
	ds.write_str(url);
	ds.write_str(file_md5);
	ds.write_u32(except_file_size);
	ds.write_u32(dl_cnt);
	
	//使用临时文件
	FILE *f = fopen(tmp_path, "wb");
	if (f == NULL)
	{
		return -1;
	}

	size_t retval = fwrite(buf, 1, ds.get_used_length(), f);
	fclose(f);
	f = NULL;

	__ASSERT_EQUAL(retval, ds.get_used_length());
	if (retval != ds.get_used_length())
	{
		unlink(path);
		__ASSERT(0);
		return -1;
	}

	//改名
	int err = rename(tmp_path, path);
	__ASSERT_EQUAL(err, 0);
	__ASSERT(access(tmp_path, 0) != 0);
	__ASSERT(access(path, 0) == 0);

	return 0;
}

/*
dl_file_path:获得下载缓存文件
except_file_md5:期望下载的md5
except_file_size:期望下载的文件大小
*/
bool RangeDlHelper::IsRangeDlInfoCorrect(const char *dl_file_path, const char* except_file_md5, int except_file_size, int dl_cnt)
{
	char path[512] = { 0 };
	GetRangeDlInfoFilePath(dl_file_path, path, sizeof(path));
	if (access(path, 0) != 0)
	{
		return false;
	}

	memory_block mem;
	bool suc = mem.read_file(path);
	if (!suc)
	{
		return false;
	}

	DataStream ds(mem.get_binary(), mem.get_binary_length());
	unsigned int magic_code = ds.read_u32();
	if (magic_code != 0x20150202)
	{
		__DEBUG_LOG("magic code for %s err, except:0x%08x, real:0x%08x", path, 0x20150202, magic_code);
		__ASSERT(0);
		return false;
	}
	char *url = ds.read_str();
	if (url == NULL)
	{
		__DEBUG_LOG("read %s err, url is null", path);
		__ASSERT(0);
		return false;
	}
	free(url);
	url = NULL;

	char* md5 = ds.read_str();
	if (md5 == NULL)
	{
		__ASSERT(0);
		return false;
	}

	unsigned int size = ds.read_u32();
	if (except_file_size != 0 && size != except_file_size)
	{
		__DEBUG_LOG("except_file_size:%d, real_file_size:%d", except_file_size, size);
		return false;
	}

	unsigned int cnt = ds.read_u32();
	if (cnt != dl_cnt)
	{
		__DEBUG_LOG("download cnt not correct");
		return false;
	}

	int ret = CompareMd5(md5, except_file_md5);
	if (ret != 0)
	{
		__DEBUG_LOG("except_file_crc:%s, real_file_crc:%s", md5, except_file_md5);
		return false;
	}

	return true;
}

void RangeDlHelper::CleanRangeDlInfo(const char *dl_file_path)
{
	char path[512] = { 0 };
	GetRangeDlInfoFilePath(dl_file_path, path, sizeof(path));
	if (access(path, 0) == 0)
	{
		int err = unlink(path);
		__ASSERT_EQUAL(err, 0);
		__ASSERT(access(path, 0) != 0);
	}
}