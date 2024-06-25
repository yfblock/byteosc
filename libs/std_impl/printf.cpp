#include <stdarg.h>
#include <kernel.h>
#include <stddef.h>
#include <stdint.h>
#include <arch.h>

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

void printf (const char *__restrict fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    for(;*fmt!='\0';fmt++) {
        if(*fmt=='%') {
            fmt++;
            switch(*fmt) {
                case 'c':
                    console_putchar(va_arg(args, int));
                    break;
                case 's':
                    printf(va_arg(args, char *));
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
    va_end(args);
}

void log(enum log_level_t level, const char *__restrict __fmt, ...) {
    if(level < CONFIG_LOG_LEVEL) {
        return;
    }
    const char *LOG_LEVEL_NAME[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};
    printf("[%s] ", LOG_LEVEL_NAME[level]);

    va_list args;
    va_start(args, __fmt);
    printf(__fmt, args);
}
