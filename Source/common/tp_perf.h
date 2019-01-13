#ifndef __FUNC_PERFORMANCE_TRAC_H__
#define __FUNC_PERFORMANCE_TRAC_H__

#include "common.h"

//函数性能统计器
class PerfModule
{
public:
	PerfModule(const char* module_name);

	~PerfModule();

public:
	void start();
	void stop(const char *msg);
private:
	void reset();

private:
	char m_module_name[256];
	long m_start_time;
};

#endif
