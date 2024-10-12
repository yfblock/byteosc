#include <aarch64.h>
#include <console.h>

const uintptr_t PSCI_0_2_FN_BASE = 0x84000000;
const uintptr_t PSCI_0_2_FN_CPU_OFF = PSCI_0_2_FN_BASE + 2;
const uintptr_t PSCI_0_2_FN_SYSTEM_OFF = PSCI_0_2_FN_BASE + 8;

inline int32_t psci_hvc_call(uintptr_t func, size_t a0, size_t a1, size_t a2) {
    register uintptr_t x0 asm("x0") = func;
    register size_t x1 asm("x1") = a0;
    register size_t x2 asm("x2") = a1;
    register size_t x3 asm("x3") = a2;

    asm volatile("hvc #0"
                 : "=r"(x0)
                 : "r"(x0), "r"(x1), "r"(x2), "r"(x3)
                 : "memory");

    return (int32_t)x0;
}

/**
 * Shutdown the platform.
 */
void shutdown() {
    int32_t ret = psci_hvc_call(PSCI_0_2_FN_SYSTEM_OFF, 0, 0, 0);
    // warn("shutdown called ret: %d", ret);
    while(true) {
        nop();
    }
}
