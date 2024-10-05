#pragma once
#include <stddef.h>
#include <stdint.h>

#define NAKED __attribute__((naked))

void console_putchar(char c);
void shutdown();
uintptr_t percpu_pointer();

extern uintptr_t _boot_stack;
extern uintptr_t _boot_stack_top;

/**
 * Get Boot Stack Size
 * @return The size of the boot stack
 */
static inline size_t boot_stack_size() {
    return (uintptr_t)&_boot_stack_top - (uintptr_t)&_boot_stack;
}

#ifndef __INCLUDE_ARCH__
#define __INCLUDE_ARCH__
#if defined(__riscv)
#include "riscv64.h"
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#include "aarch64.h"
#endif
#endif
