#ifndef __ARCH_H__
#define __ARCH_H__
#include <stddef.h>
#include <stdint.h>

void console_putchar(char c);
void shutdown();
size_t boot_stack_size();
uintptr_t percpu_pointer();

#endif
