#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <arch.h>

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
                case's':
                    printf(va_arg(args, char *));
                    break;
                case '%':
                    console_putchar('%');
                    break;
                case 'x':
                    uint8_t buffer[sizeof(size_t) * 2];
                    uint8_t len = 0;
                    size_t hex = va_arg(args, size_t);
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
