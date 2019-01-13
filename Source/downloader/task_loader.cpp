#include "task_loader.h"
#include "http_downloader.h"

RecommendTaskLoader::RecommendTaskLoader() :
	m_LastDlTipTime(0),
	m_md5(""),
	m_url(""),
	m_file_path("")
{
}

RecommendTaskLoader::~RecommendTaskLoader()
{
}

RecommendTaskLoader* RecommendTaskLoader::GetInstance()
{
	static RecommendTaskLoader* instance = NULL;
	if (instance == NULL)
	{
		instance = new RecommendTaskLoader();
	}
	return instance;
}

int RecommendTaskLoader::AddTask(const std::string& url, const std::string& md5, const std::string& file_name)
{
	std::string download_dir = "./download";
	if (access(download_dir.c_str(), 0) != 0)
	{
		if (mkdir(download_dir.c_str()) != 0)
		{
			return -1;
		}
	}

	m_md5 = md5;
	m_url = url;
	m_file_path = download_dir + "/" + file_name;
	
	DoRunDlRecommendTask();
	return 0;
}

void RecommendTaskLoader::OnTaskProgress(int pos, int total)
{
	if (total < 1000) return;
	time_t now = time(NULL);
	if (now != m_LastDlTipTime || pos == total)
	{
		m_LastDlTipTime = now;
		int percent = pos / (total / 100);
	}
}

bool RecommendTaskLoader::NeedCancel()
{
	return false;
}

int RecommendTaskLoader::DoRunDlRecommendTask()
{
	BigFileDownloader* downloader = new BigFileDownloader(this);
	int err = downloader->Download(m_url.c_str(), m_md5.c_str(), m_file_path.c_str());
	if (err != 0)
	{
		printf("download:%s failed\n", m_file_path.c_str());
		return -1;
	}

	printf("download:%s success\n", m_file_path.c_str());
	return 0;
}
