#pragma once

#include <stddef.h>
#include <stdint.h>

extern "C" uintptr_t _boot_stack;
extern "C" uintptr_t _boot_stack_top;

#define MRS(x, reg)                                                            \
    asm volatile("MRS %0, " x                                                  \
                 : "=r"(reg))

#define MSR(x, reg) asm volatile("MSR " x ", %0" ::"r"(reg))
