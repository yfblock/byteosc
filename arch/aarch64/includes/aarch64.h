#pragma once

#include <arch.h>

EXTERN uintptr_t _boot_stack;
EXTERN uintptr_t _boot_stack_top;

#define MRS(x, reg)                                                            \
    asm volatile("MRS %0, " x                                                  \
                 : "=r"(reg))
#define MSR(x, reg) asm volatile("MSR " x ", %0" ::"r"(reg))
