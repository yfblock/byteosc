#pragma once

#include <arch.h>
#include <stddef.h>
#include <stdint.h>

EXTERN uintptr_t _boot_stack;
EXTERN uintptr_t _boot_stack_top;
void trap_entry();

#define CSRW(reg, value)                                                       \
    asm volatile("csrw " reg ", %0\t\n"                                        \
                 :                                                             \
                 : "r"(value)                                                  \
                 :);
#define CSRR(reg, value)                                                       \
    asm volatile("csrr %0, " reg "\t\n"                                        \
                 : "=r"(value)                                                 \
                 :                                                             \
                 :);

/**
 * Set the trap handler for the riscv64 platform.
 */
static inline void write_stvec(void (*handler)()) {
    CSRW("stvec", (uintptr_t)handler);
}

/**
 * Get the reason of the trap.
 */
static inline size_t read_scause() {
    size_t scause;
    CSRR("scause", scause);
    return scause;
}

/**
 * Read the error address of the trap.
 */
static inline uintptr_t read_stval() {
    uintptr_t stval;
    CSRR("stval", stval);
    return stval;
}

/**
 * Get the fault ip of the exception.
 */
static inline uintptr_t read_sepc() {
    uintptr_t sepc;
    CSRR("sepc", sepc);
    return sepc;
}

/**
 * Trap Exception enum and consts.
 */
enum trap_exception {
    TRAP_EXCEPTION_INS_ADDR_MISALIGNED = 0,
    TRAP_EXCEPTION_INSTRUCTION_ACCESS_FAULT = 1,
    TRAP_EXCEPTION_ILLEGAL_INSTRUCTION = 2,
    TRAP_EXCEPTION_BREAKPOINT = 3,
    TRAP_EXCEPTION_LOAD_ACCESS_FAULT = 5,
    TRAP_EXCEPTION_AMO_ADDR_MISALIGNED = 6,
    TRAP_EXCEPTION_STORE_ACCESS_FAULT = 7,
    TRAP_EXCEPTION_ENV_CALL = 8,
    TRAP_EXCEPTION_INS_PAGE_FAULT = 12,
    TRAP_EXCEPTION_LOAD_PAGE_FAULT = 13,
    TRAP_EXCEPTION_STORE_PAGE_FAULT = 15,
};

/**
 * WFI: Waiting For Interrupt, blocking until an interrupt occurs.
 */
static inline void wfi() {
    asm volatile("wfi");
}

/**
 * NOP: No Operation.
 * This instruction performs no operation and does not consume any cycles.
 * It can be used as a placeholder or as a delay instruction.
 */
static inline void nop() {
    asm volatile("nop");
}
