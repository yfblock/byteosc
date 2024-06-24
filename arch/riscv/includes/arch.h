#ifndef __ARCH_H__
#define __ARCH_H__

// Put an character to console.
inline void console_putchar(char c) {
    __asm__(
        "li   a7, 1\n\t"
        "mv   a0, %0\n\t"
        "ecall"
        :
        :"r"(c)
        : "a0"
    );
}

#endif
