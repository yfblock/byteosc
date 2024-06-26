#ifndef __KERNEL_H__
#define __KERNEL_H__
#include <stddef.h>
#include <stdint.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define CONCAT(x, y) x y

enum log_level_t {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_WARNING = 3,
    LOG_LEVEL_ERROR = 4,
    LOG_LEVEL_OFF = 5
};

__attribute__((__format__(__printf__, 1, 2))) int
printf(const char *__restrict __fmt, ...);

__attribute__((__format__(__printf__, 2, 3))) void
log(enum log_level_t level, const char *__restrict __fmt, ...);
#define debug(fmt, ...) log(LOG_LEVEL_DEBUG, (fmt "\n"), ##__VA_ARGS__)
#define info(fmt, ...) log(LOG_LEVEL_INFO, (fmt "\n"), ##__VA_ARGS__)

#ifndef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#endif
