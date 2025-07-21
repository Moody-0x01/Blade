#ifndef LOGGER_H___
#define LOGGER_H___
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define LOGGER_FILE "./Logs.txt"

void open_logger();
void close_logger();
FILE *get_logger_file_ptr();
int log_into_f(const char *fmt, ...);

#endif // LOGGER_H___
