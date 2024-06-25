#ifndef __KERNEL_H__
#define __KERNEL_H__
#include <stddef.h>
#include <stdint.h>

enum log_level_t {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_WARNING = 3,
    LOG_LEVEL_ERROR = 4,
};

void printf (const char *__restrict __fmt, ...);

void log(enum log_level_t level, const char *__restrict __fmt, ...);

#ifndef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#endif