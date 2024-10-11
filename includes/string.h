#ifndef _HEADER_STRING__
#define _HEADER_STRING__

#include <stddef.h>

int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest, const char *src);
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest_str, const void *src_str, size_t n);
size_t strlen(const char *str);
size_t strncmp(const char *str1, const char *str2, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
void *memmove(void *dest_str, const void *src_str, size_t numBytes);
int memcmp(const void *str1, const void *str2, size_t n);

#endif
