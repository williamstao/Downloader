#ifndef __RECOMMEND_GAME_DOWNLOADER_H__
#define __RECOMMEND_GAME_DOWNLOADER_H__

#include "util.h"
#include "i_progress_adpter.h"


class RecommendTaskLoader :
	public IProgressAdapter
{
public:
	RecommendTaskLoader();
	virtual ~RecommendTaskLoader();
	static RecommendTaskLoader* GetInstance();
public:
	int AddTask(const std::string& url,const std::string& md5, const std::string& file_name);
	
public:     //from IProgressAdapter
	virtual void OnTaskProgress(int pos, int total);
	virtual bool NeedCancel();

private:
	int DoRunDlRecommendTask();
	
private:
	
	std::string m_url;
	std::string m_md5;
	std::string m_file_path;
	time_t m_LastDlTipTime;
};

#endif