#pragma once

#include <stddef.h>
#include <stdint.h>

#define NAKED __attribute__((naked))
#define EXTERN extern "C"

void console_putchar(char c);
void shutdown();
uintptr_t percpu_pointer();

EXTERN uintptr_t _boot_stack;
EXTERN uintptr_t _boot_stack_top;

/**
 * Get Boot Stack Size
 * @return The size of the boot stack
 */
static inline size_t boot_stack_size() {
    return (uintptr_t)&_boot_stack_top - (uintptr_t)&_boot_stack;
}
