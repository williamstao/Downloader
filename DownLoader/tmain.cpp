#include "../Source/downloader/util.h"
#include "../Source/downloader/task_loader.h"
#include "../Source/downloader/curl_helper.h"
#include <fstream>
#include <string>
#include <iostream>

int dlCnt = 0;
int readfile(const char* filepath, std::string& url, std::string& md5, std::string& name, std::string& dlCnt)
{
	if (filepath == NULL || filepath[0] == 0)
	{
		return -1;
	}

	if (access(filepath, 0) != 0)
		return -1;

	std::ifstream in(filepath);
	std::string line;
	if (in.good())
	{
		while (std::getline(in, line)) // line中不包括每行的换行符
		{
			if (line.length() == 0)
			{
				continue;
			}
			const char* pos = strstr(line.c_str(), "url=");
			if (pos != NULL)
			{
				url = std::string(pos + 4);
			}
			pos = strstr(line.c_str(), "md5=");
			if(pos != NULL)
			{
				md5 = std::string(pos + 4);
			}
			pos = strstr(line.c_str(), "name=");
			if(pos !=NULL)
			{
				name = std::string(pos + 5);
			}
			pos = strstr(line.c_str(), "dlCnt=");
			if(pos != NULL)
			{
				dlCnt = std::string(pos + 6);
			}
		}
		in.close();
	}
	else
	{
		return -1;
	}
	return 0;
}

class my_curl
{
public:
	my_curl()
	{
		curl_global_init(CURL_GLOBAL_ALL);
	}

	~my_curl()
	{
		curl_global_cleanup();
	}
};

int main(int argc, char** argv)
{
	my_curl curl_init;
	if (argc != 4)
	{
		printf("argument error");
		return -1;
	}

	const char* url = argv[1];
	const char* md5 = argv[2];
	const char* name = argv[3];
	if (url == NULL || md5 == NULL || name == NULL)
	{
		printf("check argument error\n");
		return -1;
	}

	__DEBUG_LOG("start download");
	RecommendTaskLoader* loader = RecommendTaskLoader::GetInstance();
	loader->AddTask(url, md5, name);
	
	return 0;
}