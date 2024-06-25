#include <arch.h>
#include <console.h>
#include <stddef.h>
#include <stdint.h>

__always_inline void ecall(size_t EID, size_t arg0) {
    register uintptr_t a0 asm ("a0") = (uintptr_t) arg0;
    register uintptr_t a7 asm ("a7") = (uintptr_t) EID;
    asm volatile (
        "ecall \n\t"
        : "+r" (a0)
        : "r"(a7), "r"(a0)
        : "memory"
    );
}

void console_putchar(char c) {
    ecall(0x1, c);
}

extern "C" void boot_main(size_t hart_id, uintptr_t dtb) {
    int *t = new int[3];
    t[0] = '3';
    delete t;

    for(int i = 0;i < 3;i++) 
        console_putchar(t[i]);

    puts((char *)"Hello, world!\n");
    int main(size_t, uintptr_t);
    main(hart_id, dtb);
}
