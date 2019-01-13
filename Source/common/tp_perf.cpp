#include "tp_perf.h"
#include <windows.h>

PerfModule::PerfModule(const char* module_name)
{
	m_start_time = GetTickCount();
	if (module_name == NULL || strlen(module_name) == 0)
	{
		module_name = "Unknown Module";
	}

	memset(m_module_name, 0, sizeof(m_module_name));
	size_t a = strlen(module_name);
	size_t b = sizeof(m_module_name) - 1;
	strncpy(m_module_name, module_name, a>b ? a : b);
}

PerfModule::~PerfModule()
{
	
}

void PerfModule::reset()
{
	m_start_time = GetTickCount();
}

void PerfModule::start()
{
	reset();
}

void PerfModule::stop(const char* msg)
{
	if (msg == NULL || strlen(msg) == 0)
	{
		reset();
		return;
	}

	long end = GetTickCount();
	long diff = end > m_start_time ? (end - m_start_time) : 0;
	__DEBUG_LOG("[%s] time:%d ms", msg, diff);
	reset();
}





