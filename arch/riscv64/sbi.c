#include <arch.h>

#define EID_SBI_SET_TIMER 0
#define EID_SBI_CONSOLE_PUTCHAR 1
#define EID_SBI_CONSOLE_GETCHAR 2
#define EID_SBI_SHUTDOWN 8

/**
 * Call to SBI
 */
inline void ecall(size_t EID, size_t arg0) {
    register uintptr_t a0 asm("a0") = (uintptr_t)arg0;
    register uintptr_t a7 asm("a7") = (uintptr_t)EID;
    asm volatile("ecall \n\t"
                 : "+r"(a0)
                 : "r"(a7), "r"(a0)
                 : "memory");
}

/**
 * Put a character to console. Uart
 * @param c The character
 */
void sbi_putchar(char c) {
    ecall(EID_SBI_CONSOLE_PUTCHAR, c);
}

/* Shutdown the machine */
void shutdown() {
    ecall(EID_SBI_SHUTDOWN, 0);
}
