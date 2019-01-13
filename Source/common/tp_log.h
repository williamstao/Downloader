#ifndef __LOG_H_
#define __LOG_H_

#include "build_option.h"
#include "common.h"
#include <stdio.h>

void tp_log_debug(char* file, int line, _In_z_ _Printf_format_string_ char const* const fmt, ...);

#endif