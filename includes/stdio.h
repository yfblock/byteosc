#pragma once

#include <console.h>
#include <stdarg.h>

int vprintf(const char *format, va_list va);
int sprintf(char *buffer, const char *format, ...);

// __attribute__((__format__(__printf__, 1, 2)))
int printf(const char *__restrict __fmt, ...);
