#include <arch.h>
#include <console.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void puts(const char *str) {
    for(; *str != '\0'; str++)
        console_putchar(*str);
}

const char   *LOG_LEVEL_NAME[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};

// ANSI escape codes.
// https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
const uint8_t color_code[]     = {90, 32, 34, 93, 31};
void          log(enum log_level_t level, const char *__restrict __fmt, ...) {
    if(level < CONFIG_LOG_LEVEL) {
        return;
    }
    printf("\033[%dm[%s] ", color_code[level], LOG_LEVEL_NAME[level]);
    // Pass va_list to vprintf.
    va_list args;
    va_start(args, __fmt);
    vprintf(__fmt, args);
    va_end(args);
    puts((char *)"\033[0m");
}
