#include "curl_helper.h"

void curl_setopt_default(CURL *curl)
{
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	//curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	//curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)10240);//降速测试
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
}

//test curl
struct MemoryStruct {
	char *memory;
	size_t size;
};

static CURL *curl = NULL;
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	double length = 0;
	curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &length);
	if (length > 0)
	{
		printf("length:%.0f\n", length);
		return 0;
	}
		
	return realsize;
}

void curl_example(const char* url)
{
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (curl)
	{
		struct MemoryStruct chunk;
		chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
		chunk.size = 0;

		curl_setopt_default(curl);
		curl_easy_setopt(curl, CURLOPT_URL, url);

		char range[64];
		sprintf(range, "%d-", 20);
		curl_easy_setopt(curl, CURLOPT_RANGE, range);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		CURLcode retval = curl_easy_perform(curl);
		printf("retval:%d\n", retval);
		int retcode = 0;
		retval = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		printf("retval:%d\n",retval);
		printf("retcode:%d\n",retcode);
		
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
}