#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <direct.h>
#include <inttypes.h>  
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <iterator>
#include <windows.h>  
#include <fstream>  
#include <iostream>  
#include <string>  
#include <vector> 
#include <sstream>

#include "tp_log.h"

void tp_assert(int express, const char *file, int line);
void tp_assert_equal(int a, int b, const char *file, int line);

#define __DEBUG_LOG(format, ...)  tp_log_debug(__FILE__, __LINE__, format, __VA_ARGS__)
#define __ASSERT(express) tp_assert(express, __FILE__, __LINE__)
#define __ASSERT_EQUAL(a, b) tp_assert_equal(a, b, __FILE__, __LINE__)

#endif