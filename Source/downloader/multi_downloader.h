#ifndef __MULTI_DOWNLOADER_H__
#define __MULTI_DOWNLOADER_H__
#include "util.h"
#include "i_progress_adpter.h"

enum
{
	Dl_Unfinish = 0,
	Dl_Ok = 1,
	Dl_Err =2
};

class Downloader : public TPThread
{
public:
	Downloader(IMultiDownloaderAdapter* adapter);
	virtual ~Downloader();

public:
	int AddDownloadTask(int start, int end, const std::string& url, int index, const std::string& tmp_file, uint32_t chunk_size);
	virtual void DoRun();
	int IsdownloadOver();
	
	size_t CurlDownloadCallBack(void* ptr, size_t size, size_t nmemb);
private:
	static size_t curl_download_callback(void* ptr, size_t size, size_t nmemb, void* param);
	void DoMultiDownloader();
	void StopDownload();
	
	IMultiDownloaderAdapter *m_adapter;
	std::string m_url;
	uint32_t m_start;
	uint32_t m_end;
	uint32_t m_offset;
	uint32_t m_chunk_size;
	int m_index;
	FILE* m_file;
	int m_downloadOver;
	std::string m_temp_file;
};



#endif