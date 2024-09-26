#ifndef _HEADER_STRING__
#define _HEADER_STRING__

#include <stddef.h>

int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest, const char *src);

extern "C" void *memset(void *s, int c, size_t n);

#endif
