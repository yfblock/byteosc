#include <arch.h>
#include <assert.h>
#include <console.h>
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
    while(true) {
        if(*str1 == *str2 && *str1 != '\0') {
            str1++;
            str2++;
            continue;
        }
        return *str1 - *str2;
    }
}

#define UCHAR_MAX 255
#define ALIGN (sizeof(size_t))
#define ONES ((size_t) - 1 / UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) (((x) - ONES) & ~(x) & HIGHS)

char *__stpcpy(char *restrict d, const char *restrict s) {
#ifdef __GNUC__
    typedef size_t __attribute__((__may_alias__)) word;
    word *wd;
    const word *ws;
    if((uintptr_t)s % ALIGN == (uintptr_t)d % ALIGN) {
        for(; (uintptr_t)s % ALIGN; s++, d++)
            if(!(*d = *s))
                return d;
        wd = (void *)d;
        ws = (const void *)s;
        for(; !HASZERO(*ws); *wd++ = *ws++)
            ;
        d = (void *)wd;
        s = (const void *)ws;
    }
#endif
    for(; (*d = *s); s++, d++)
        ;

    return d;
}

char *strcpy(char *restrict dest, const char *restrict src) {
    __stpcpy(dest, src);
    return dest;
}

int strncmp(const char *_l, const char *_r, size_t n) {
    const unsigned char *l = (void *)_l, *r = (void *)_r;
    if(!n--)
        return 0;
    for(; *l && *r && n && *l == *r; l++, r++, n--)
        ;
    return *l - *r;
}

// musl, typedef int (*cmpfun)(const void *, const void *);
typedef int (*__compar_fn_t)(const void *, const void *);
typedef int (*__compar_d_fn_t)(const void *, const void *, void *);
__attribute__((weak)) void qsort_r(void *base, size_t nel, size_t width,
                                   __compar_d_fn_t comp, void *arg) {
    size_t wgap, i, j, k;
    char tmp;

    if((nel > 1) && (width > 0)) {
        // check for overflow
        // assert(nel <= ((size_t)(-1)) / width);
        assert(nel <= ((size_t)(-1)) / width);
        wgap = 0;
        do {
            wgap = 3 * wgap + 1;
        } while(wgap < (nel - 1) / 3);
        /* From the above, we know that either wgap == 1 < nel or */
        /* ((wgap-1)/3 < (int) ((nel-1)/3) <= (nel-1)/3 ==> wgap <  nel. */
        wgap *= width; /* So this can not overflow if wnel doesn't. */
        nel *= width;  /* Convert nel to 'wnel' */
        do {
            i = wgap;
            do {
                j = i;
                do {
                    register char *a;
                    register char *b;

                    j -= wgap;
                    a = j + ((char *)base);
                    b = a + wgap;
                    if((*comp)(a, b, arg) <= 0) {
                        break;
                    }
                    k = width;
                    do {
                        tmp = *a;
                        *a++ = *b;
                        *b++ = tmp;
                    } while(--k);
                } while(j >= wgap);
                i += width;
            } while(i < nel);
            wgap = (wgap - width) / 3;
        } while(wgap);
    }
}

void qsort(void *base, size_t nel, size_t width, __compar_fn_t comp) {
    return qsort_r(base, nel, width, (__compar_d_fn_t)comp, NULL);
}

void *memcpy(void *dest_str, const void *src_str, size_t n) {
    for(int i = 0; i < n; i++) {
        *((char *)dest_str + i) = *((char *)src_str + i);
    }
}

int memcmp(const void *v1, const void *v2, size_t n) {
    const uint8_t *s1, *s2;

    s1 = v1;
    s2 = v2;
    while(n-- > 0) {
        if(*s1 != *s2)
            return *s1 - *s2;
        s1++, s2++;
    }

    return 0;
}

void *memmove(void *dst, const void *src, size_t n) {
    const char *s;
    char *d;

    s = src;
    d = dst;
    if(s < d && s + n > d) {
        s += n;
        d += n;
        while(n-- > 0)
            *--d = *--s;
    } else
        while(n-- > 0)
            *d++ = *s++;

    return dst;
}

char *strncpy(char *s, const char *t, int n) {
    char *os;

    os = s;
    while(n-- > 0 && (*s++ = *t++) != 0)
        ;
    while(n-- > 0)
        *s++ = 0;
    return os;
}
