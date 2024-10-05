#include <aarch64.h>
#include <arch.h>

/**
 * @brief Get percpu pointer
 * @return The start address of this percpu area
 */
uintptr_t percpu_pointer() {
    register uintptr_t x0 asm("x0");
    MRS("TPIDR_EL1", x0);
    return x0;
}

/**
 * @brief Write percpu pointer
 * @param ptr The start address of this percpu area
 */
void set_percpu_pointer(uintptr_t ptr) {
    register uintptr_t x0 asm("x0") = ptr;
    MSR("TPIDR_EL1", x0);
}
