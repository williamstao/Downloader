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
	/*const char* filepath = "";
	std::string url = "";
	std::string md5 = "";
	std::string name = "";
	std::string cnt = "";
	if (argc != 2)
	{
		printf("downloader configPath\n");
		filepath = "C:\\Users\\williamtao\\Desktop\\config.txt";
	}
	else
	{
		filepath = argv[1];
	}

	int err = readfile(filepath, url, md5, name, cnt);
	if (err != 0)
	{
		return -1;
	}

	printf("url:%s\n",url.c_str());
	printf("md5:%s\n", md5.c_str());
	printf("name:%s\n",name.c_str());
	printf("cnt:%s\n", cnt.c_str());

	for (int i = 0; i < cnt.length(); i++)
	{
		if (!isdigit(cnt[i]))
		{
			return -1;
		}
	}
	dlCnt = std::stoi(cnt);*/

	/*const char* url = "http://dlied5.myapp.com/myapp/1104466820/sgame/2017_com.tencent.tmgp.sgame_h7916_1.42.1.20_5eb4f3.apk";
	const char* md5 = "1ea728e92a6aa8186c02358f22f06659";
	const char* name = "2017_com.tencent.tmgp.sgame_h7916_1.42.1.20_5eb4f3.apk";*/

	//const char* url = "http://dlied5.myapp.com/myapp/100539858/feiji/2017_com.tencent.feiji_h100_1.0.66_666477.apk";
	//const char* md5 = "24dfe3663290f2484a8961e1cbd3da9e";
	//const char* name = "2017_com.tencent.feiji_h100_1.0.66_666477.apk";

	//const char* url = "http://down.mtp.oa.com/storage/add_shell/apk_file/201812/29/10/94773_8204a26147843cb15b82b4ee61ace8e0_17087/WePop_2018-12-29-10-58_0.0.3.65.apk?t=vx6h5BULwxCXrn7oA6ZRzHbBo54PzzkEO0SMCb1BR5p7/4coYHREfdUHdf6zhzApjRgfpoLJJYSxKEAMWjlziJUkuzj8R9BlQUa/i/M0LagHj5lD9pQPW3txWpOABLDEKOyH3LaOvgJ/jFoicEwpnQ==";
	//const char* md5 = "8204a26147843cb15b82b4ee61ace8e0";
	//const char* name = "WePop_2018-12-29-10-58_0.0.3.65.apk";

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