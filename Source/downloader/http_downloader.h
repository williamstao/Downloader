#ifndef __HTTP_DOWNLOADER_H__
#define __HTTP_DOWNLOADER_H__
#include "../common/common.h"
#include "../common/tp_lock.h"
#include "../common/tp_thread.h"
#include "curl_helper.h"
#include "i_progress_adpter.h"
#include "multi_downloader.h"

class RangeDlHelper
{
public:
	/**
	* @dl_file_path: 要下载的文件的本地路径, WriteRangeDlInfo将根据dl_file_path组成出断点续传文件路径
	*/
	int WriteRangeDlInfo(const char *url, const char *dl_file_path, const char* file_md5, unsigned int except_file_size, int dl_cnt);
	bool IsRangeDlInfoCorrect(const char *dl_file_path, const char* except_file_md5, int except_file_size, int dl_cnt);
	void CleanRangeDlInfo(const char *dl_file_path);

private:
	void GetRangeDlInfoFilePath(const char *dl_file_path, char *path, size_t buf_size);
};
#define tp_dlcnt 5
/**
* @class   BigFileDownloader
* @brief   大文件下载类, 下载内容不全部塞入内存，而是直接写入指定硬盘文件
*/
class BigFileDownloader:public IMultiDownloaderAdapter
{
public:
	BigFileDownloader(IProgressAdapter *adapter);
	~BigFileDownloader();

	enum {
		DlErr_Ok = 0,
		DlErr_Param = 1,
		DLErr_Malloc = 2,
		DLErr_Rename = 3,
		DLErr_NotWifi = 4,
		DLErr_OpenTmpFile = 5,
		DLErr_InitGlobalCurl = 6,
		DLErr_InitCurl = 7,
		DLErr_Retcode = 8,
		DLErr_CheckMd5 = 9,
		DlErr_Rename2 = 10,
		DlErr_DiskFull = 11,
		DlErr_PerpareToDl = 99,
	};

public:
	void SetAdapter(IProgressAdapter *adapter);
	int Download(const char *url, const char* md5, const char *local_path);
	int DoDownload(const char *url, const char* md5, const char *local_path);
	bool IsFromCache() const { return m_from_cache; };
	int GetContentLength(const char* url);
	int DownloadPlist();
	int MergeDownloadFile();
	size_t CurlDownloadCallBack(void* ptr, size_t size, size_t nmemb);
	
public:
	void SetCancel(bool isCancel);
	bool NeedCancel();
	int DownloadPlistFile(int index, int offset, int num);
	static size_t curl_download_callback(void* ptr, size_t size, size_t nmemb, void* param);

private:
	void CleanUp(const char *tmp_path);

private:
	IProgressAdapter *m_adapter;
	
	CURL *m_curl;
	int m_total_size;
	bool m_is_range_dl;     //是否为断点续传

	std::string m_url;
	std::string m_tmp_path;
	std::string m_except_file_md5;

	//错误码
	int m_err_code;
	int m_errno;
	int m_retval;
	int m_retcode;
	
	bool m_from_cache;
	TPMutexLock m_Lock;
	bool m_needcancel;
	int m_offset;

	std::list<Downloader*> dLoader;
};

#endif