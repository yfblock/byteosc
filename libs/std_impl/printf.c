#include <arch.h>
#include <console.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

void print_number(size_t hex) {
    uint8_t buffer[32];
    uint8_t len = 0;
    while(hex != 0) {
        buffer[len++] = "0123456789abcdef"[hex % 10];
        hex /= 10;
    }
    for(uint8_t i = 0; i < len; i++) {
        console_putchar(buffer[len - i - 1]);
    }
    if(len == 0) {
        console_putchar('0');
    }
}

void print_hex(size_t hex) {
    uint8_t buffer[sizeof(size_t) * 2];
    uint8_t len = 0;
    while(hex != 0) {
        buffer[len++] = "0123456789abcdef"[hex & 0xf];
        hex >>= 4;
    }
    for(uint8_t i = 0; i < len; i++) {
        console_putchar(buffer[len - i - 1]);
    }
    if(len == 0) {
        console_putchar('0');
    }
}

void puts(const char *str) {
    for(; *str != '\0'; str++)
        console_putchar(*str);
}

void vprintf(const char *fmt, va_list args) {
    for(; *fmt != '\0'; fmt++) {
        if(*fmt == '%') {
            fmt++;
            switch(*fmt) {
            case 'c':
                console_putchar(va_arg(args, int));
                break;
            case 's':
                puts(va_arg(args, char *));
                break;
            case '%':
                console_putchar('%');
                break;
            case 'd':
                print_number(va_arg(args, size_t));
                break;
            case 'x':
                print_hex(va_arg(args, size_t));
                break;
            default:
                break;
            }
        } else {
            console_putchar(*fmt);
        }
    }
}

int printf(const char *__restrict fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    return 0;
}

const char *LOG_LEVEL_NAME[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};

// ANSI escape codes.
// https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
const uint8_t color_code[] = {90, 32, 34, 93, 31};
void log(enum log_level_t level, const char *__restrict __fmt, ...) {
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
