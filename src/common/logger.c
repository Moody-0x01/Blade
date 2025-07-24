#include <logger.h>
static FILE *stdl = NULL;


void open_logger(void)
{
	stdl = fopen(LOGGER_FILE, "a+");
}

FILE *get_logger_file_ptr(void)
{
	if (stdl)
		return stdl;
	stdl = fopen(LOGGER_FILE, "a+");
	return stdl;
}

void close_logger(void)
{
	if (stdl) {
		fclose(stdl);
		stdl = NULL;
	}
}

int log_into_f(const char *fmt, ...)
{
	int n = 0;
	va_list args;
	char _fmt_buff[4096] = { 0 };

	sprintf(_fmt_buff, "%s: ", getenv("TERM"));

	if (fmt)
		strcat(_fmt_buff, fmt);
	else
		strcat(_fmt_buff, "null");

	va_start(args, fmt);
	open_logger();
	n = vfprintf(stdl, _fmt_buff, args);
	close_logger();
	return n;
}
