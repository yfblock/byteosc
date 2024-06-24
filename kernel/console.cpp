#include <arch.h>

void puts(char *str) {
    for(;*str != '\0';str++) 
        console_putchar(*str);
}
