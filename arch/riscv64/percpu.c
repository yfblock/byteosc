#include <arch.h>

/**
 * @brief Get percpu pointer
 * @return The start address of this percpu area
 */
uintptr_t percpu_pointer() {
    register uintptr_t a0 __asm__("a0");
    __asm__("mv %0, gp\n\t"
            : "=r"(a0)
            :
            : "memory");
    return a0;
}

/**
 * @brief Write percpu pointer
 * @param ptr The start address of this percpu area
 */
void set_percpu_pointer(uintptr_t ptr) {
    register uintptr_t a0 __asm__("a0") = ptr;
    __asm__("mv gp, %0\n\t"
            :
            : "r"(a0)
            : "memory");
}
