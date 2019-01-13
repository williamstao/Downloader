#include "common.h"

void tp_assert(int express, const char* file, int line)
{
	if (express == 0)
	{
		__DEBUG_LOG("__ASSERT err, file:%s, ln:%d", file, line);
	}
}

void tp_assert_equal(int a, int b, const char* file, int line)
{
	if (a != b)
	{
		__DEBUG_LOG("__ASSERT err, a:%d, b:%d, file:%s, ln:%d", a, b, file, line);
	}
}

