#include <arch.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Get the length of the string
 * @param s The string will be handled.
 * @return The length of the string s.
 */
size_t strlen(const char *s) {
    register size_t len = 0;
    register uintptr_t buffer = (uintptr_t)s;
    for(; *(char *)buffer != '\0'; buffer++)
        len++;
    return len;
}

/**
 * Set the memory range between s-(s+n) with c
 * @param s The start address of the memory range
 * @param c The value will be filled with
 * @param n The size of the memory range will be set.
 * @return The original address of the memory range. equals to param s
 */
void *memset(void *s, int c, size_t n) {
    register uintptr_t buffer = (uintptr_t)s;
    register char value = c;
    register size_t number = n;
    for(; number > 0; buffer++) {
        number--;
        *(char *)buffer = c;
    }
    return (void *)buffer;
}

/**
 * @param str1 The first string will be compared
 * @param str2 The second string will be compared
 * @return Return 0 if the str1 equals str2. return negative if str1 is
 *         less than str2, return positive if str1 is bigger than str2.
 */
int strcmp(const char *str1, const char *str2) {
    while (true) {
        if (*str1 == *str2 && *str1 != '\0') {
            str1++;
            str2++;
            continue;
        }
        return *str1 - *str2;
    }
}
