#pragma once

#include <define.h>
#include <stddef.h>
#include <stdint.h>

#define talloc(num, ty) calloc(num, sizeof(ty))
#define MAX_BUDDY_HEADER_BITS 32
#define MIN_UNIT_BITS 5
#define MIN_UNIT_SIZE (1 << MIN_UNIT_BITS)

extern uint8_t heap[HEAP_SIZE];

/* Alloc memory from the buddy system */
void *malloc(size_t size);
void *calloc(size_t num, size_t size);
void *kalloc(size_t count);
/* Free memory  */
void free(void *ptr);
void free_len(void *ptr, size_t len);
/* Memory add  */
void add_heap_range(uintptr_t addr, size_t size);

void add_frame_range(uintptr_t addr_s, uintptr_t addr_e);

void dump_heap();
