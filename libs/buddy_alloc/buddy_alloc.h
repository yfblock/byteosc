#ifndef __BUDDY_ALLOC_H__
#define __BUDDY_ALLOC_H__

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BUDDY_HEADER_BITS 32

void mem_add(uintptr_t addr, size_t size);

#endif
