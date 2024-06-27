#ifndef __BUDDY_ALLOC_H__
#define __BUDDY_ALLOC_H__

#ifndef HEAP_SIZE
#define HEAP_SIZE 0x200000
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#define MAX_BUDDY_HEADER_BITS 32

extern uint8_t heap[HEAP_SIZE];

void mem_add(uintptr_t addr, size_t size);

#endif
