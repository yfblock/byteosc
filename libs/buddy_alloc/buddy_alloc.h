#pragma once

#ifndef HEAP_SIZE
#define HEAP_SIZE 0x200000
#endif

#include <stddef.h>
#include <stdint.h>
// #include <string.h>
#define MAX_BUDDY_HEADER_BITS 32
#define MIN_UNIT_BITS 5
#define MIN_UNIT_SIZE (1 << MIN_UNIT_BITS)

extern uint8_t heap[HEAP_SIZE];

/* Alloc memory from the buddy system */
void *malloc(size_t size);
/* Free memory  */
void free(void *ptr);
void free_len(void *ptr, size_t len);
/* Memory add  */
void mem_add(uintptr_t addr, size_t size);
