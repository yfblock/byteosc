#pragma once

#include <arch.h>

extern uintptr_t _boot_stack;
extern uintptr_t _boot_stack_top;

#define MRS(x, reg)               \
    __asm__ volatile("MRS %0, " x \
                     : "=r"(reg))
#define MSR(x, reg) __asm__ volatile("MSR " x ", %0" ::"r"(reg))

/**
 * WFI: Waiting For Interrupt, blocking until an interrupt occurs.
 */
static inline void wfi() {
    __asm__ volatile("wfi");
}

/**
 * NOP: No Operation.
 * This instruction performs no operation and does not consume any cycles.
 * It can be used as a placeholder or as a delay instruction.
 */
static inline void nop() {
    __asm__ volatile("nop");
}
