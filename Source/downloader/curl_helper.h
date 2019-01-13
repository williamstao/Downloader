#ifndef __CURL_HELPER_H__
#define __CURL_HELPER_H__
#include "../common/common.h"
#include "curl/curl.h"
#include "curl/easy.h"

void curl_setopt_default(CURL *curl);
void curl_example(const char* url);

#endif