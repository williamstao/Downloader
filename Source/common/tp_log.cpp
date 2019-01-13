#include "tp_log.h"
#include "file_helper.h"
#include "time.h"

static char* log_path = "./log/tplog.log";
static void init_log_path()
{
	const char* log_dir = "./log";
	if (access(log_dir, 0) != 0)
	{
		mkdir(log_dir);
	}
}

static void tp_clean_log()
{
	static bool is_init = false;
	if (!is_init)
	{
		init_log_path();
		unlink(log_path);
		is_init = true;
	}
}

static void get_file_base_name(const char* file, char* name, size_t size)
{
	if (file == NULL || name == NULL)
	{
		return;
	}

	const char* tmp = file;
	file += strlen(file);
	while (file > tmp)
	{
		if (*file == '/' || *file == '\\')
		{
			file++;
			break;
		}
		file--;
	}
	strncpy(name, file, size - 1);
}

void tp_log(const char* file, int line, char* buf, size_t buf_len)
{
	tp_clean_log();
	char file_base_name[0xff] = { 0 };
	get_file_base_name(file, file_base_name, sizeof(file_base_name));
	FILE* fd = fopen(log_path, "ab+");
	if (fd != NULL)
	{
		char data[64] = {0};
		memset(data, '-', sizeof(data) - 1);

		char timebuf[64] = {0};
		time_t t = time(NULL);
		struct tm* p = localtime(&t);
		snprintf(timebuf, sizeof(timebuf), "%04d %02d/%02d %02d:%02d:%02d---%s: %d   ", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, file_base_name, line);

		memcpy(data, timebuf, strlen(timebuf));

		fwrite(data, 1, strlen(data), fd);
		fwrite(buf, 1, strlen(buf), fd);
		fwrite("\r\n", 1, 2, fd);
		fclose(fd);
	}
}

void tp_log_debug(char* file, int line, _In_z_ _Printf_format_string_ char const* const fmt, ...)
{
	char buf[2048] = {0};
	va_list arg_ls;
	va_start(arg_ls, fmt);
	vsnprintf(buf, sizeof(buf), fmt, arg_ls);
	va_end(arg_ls);

	tp_log(file, line, buf, strlen(buf));
}