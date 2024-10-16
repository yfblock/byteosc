#pragma once

#include <arch.h>
// Put string to the console.
void puts(const char *);

enum log_level_t {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_WARNING = 3,
    LOG_LEVEL_ERROR = 4,
    LOG_LEVEL_OFF = 5
};

// __attribute__((__format__(__printf__, 2, 3)))
void log(enum log_level_t level, const char *__restrict __fmt, ...);
#define debug(fmt, ...) log(LOG_LEVEL_DEBUG, (fmt "\n"), ##__VA_ARGS__)
#define info(fmt, ...) log(LOG_LEVEL_INFO, (fmt "\n"), ##__VA_ARGS__)
#define warn(fmt, ...) log(LOG_LEVEL_WARNING, (fmt "\n"), ##__VA_ARGS__)
#define error(fmt, ...) log(LOG_LEVEL_ERROR, (fmt "\n"), ##__VA_ARGS__)
#define panic(fmt, ...)                                                        \
    do {                                                                       \
        error(fmt, ##__VA_ARGS__);                                             \
        shutdown();                                                            \
    } while(0)

#ifndef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL LOG_LEVEL_DEBUG
#endif
