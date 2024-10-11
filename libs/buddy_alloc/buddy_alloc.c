#include <buddy_alloc.h>
#include <common.h>
#include <console.h>
#include <string.h>
#define HEADER_SIZE sizeof(mem_header_t)
#define IN_RANGE(a, rs, re) (((a) >= (rs) && (a) < (re)))

/* Heap allocator, heap */
uint8_t heap[HEAP_SIZE] = {0};

/* Define BuddyLinked Node Type */
typedef struct buddy_linked buddy_linked_t;
/* Define BuddyLinked Node Real Struct */
struct buddy_linked {
    buddy_linked_t *next;
};

/**
 * The system container for the buddy system
 * Use this to define multiple memory alloc system.
 * The different is unit_size. This indicates the size
 * of the last header.
 */
typedef struct {
    buddy_linked_t header[MAX_BUDDY_HEADER_BITS];
    size_t unit_size;
} buddy_system_t;

typedef struct {
    /* Unit validate, equals the real raw pointer. */
    void *ptr_validate;
    /* Size of this memory unit. */
    size_t size;
} mem_header_t;

/**
 * Get the memory size with memory header.
 */
inline size_t size_with_header(const size_t size) {
    return size + HEADER_SIZE;
}

/* Static assert or Compiler assert */
// static_assert(
//     sizeof(BuddyLinked) == sizeof(intptr_t),
//     "sizeof(BuddyLinkedAssert) Must be same as the sizeof(uintptr_t)");

// static buddy_linked_t buddy_header[MAX_BUDDY_HEADER_BITS] = {nullptr};
// /* Page Allocator Buddy Header */
// static buddy_linked_t page_header[MAX_BUDDY_HEADER_BITS] = {nullptr};
static buddy_system_t heap_buddy = {
    .header = {nullptr},
    .unit_size = MIN_UNIT_SIZE
};

static buddy_system_t page_buddy = {
    .header = {nullptr},
    .unit_size = PAGE_SIZE
};

/**
 * Get the index level through the allocation size.
 * @param buddy The buddy system will be operated
 * @param size The size of the allocation
 * @return The index level, 0 is 8 bytes, 1 is 16 bytes...
 */
inline size_t header_index(const buddy_system_t *buddy, const size_t size) {
    size_t index = 0;
    // End when l >= size, so l is the min size
    // available when satisfying the size limit
    for(size_t l = buddy->unit_size; l < size; l <<= 1)
        index++;
    return index;
}

/**
 * Add a buddy node to the header list
 * @param buddy The buddy system will be operated
 * @param index buddy header index
 * @param addr the memory block will be added
 * @return void
 */
void add_node(buddy_system_t* buddy, const size_t index, const uintptr_t addr) {
    const auto node = (buddy_linked_t *)addr;
    buddy_linked_t *link = &buddy->header[index];
    while(link->next != NULL && (uintptr_t)link->next < (uintptr_t)node) {
        link = link->next;
    }
    node->next = link->next;
    link->next = node;
    // TOOD：merge the neighbor memory in the same node.
    // such as: merge 0x400(8 bytes) and 0x408(bytes), result is 0x400(16
    // bytes).
}

/**
 * Add memory to buddy header list
 * @param addr The address of the memory block
 * @param size The size of the memory block
 */
void add_range(buddy_system_t *buddy, uintptr_t addr, const size_t size) {
    uintptr_t end = addr + size;
    // align to 8 bytes.
    addr = (addr + (buddy->unit_size - 1)) & ~(buddy->unit_size - 1);
    end &= ~(buddy->unit_size - 1);
    // add to the buddy header list
    for(size_t index = 0; addr < end; index++) {
        const size_t bit_and = buddy->unit_size << index;
        if((addr & bit_and) != 0) {
            add_node(buddy, index, addr);
            addr += bit_and;
        }
        if((end & bit_and) != 0) {
            end -= bit_and;
            add_node(buddy, index, end);
        }
    }
}

/**
 * Add memory to buddy header list
 * @param addr The address of the memory block
 * @param size The size of the memory block
 */
void add_heap_range(uintptr_t addr, const size_t size) {
    add_range(&heap_buddy, addr, size);
}

void *alloc_node(buddy_system_t *buddy, size_t size) {
    size_t index = header_index(buddy, size);

    // alloc buddy node
    buddy_linked_t* node = buddy->header[index].next;
    // find the first available buddy node
    while(node == NULL) {
        node = buddy->header[++index].next;
    }

    buddy->header[index].next = node->next;
    uintptr_t addr = (uintptr_t)node;
    size_t available = buddy->unit_size << index;

    // add the last bytes to the buddy.
    if(available != size)
        add_range(buddy, addr + size, available - size);

    return (void *)addr;
}

/**
 * Add the pages to frame buddy allocator.
 * @param addr_s the start address of the added range.
 * @param addr_e the end address of the added range.
 */
void add_frame_range(uintptr_t addr_s, uintptr_t addr_e) {
    extern void* _end;
    const uintptr_t end_addr = (uintptr_t)&_end;
    if(IN_RANGE(end_addr, addr_s, addr_e)) {
        addr_s = (end_addr + PAGE_MASK) & ~PAGE_MASK;
    }
    add_range(&page_buddy, addr_s, addr_e - addr_s);
}

/**
 * Allocate count pages memory aligned with PAGE_SIZE
 * @param count the count page number will be allocated
 * @return the pointer of the allocated memory
 */
void *kalloc(size_t count) {
    void *ptr = alloc_node(&page_buddy, count * PAGE_SIZE);
    return ptr;
}

/**
 * Release a physical pages.
 * @param ptr The pointer of the page that will be released.
 * @param count Release count pages.
 */
void kfree(void *ptr, size_t count) {
    error("kfree is not implemented!");
}

/**
 * alloc size of a memory block, aligned to 8 bytes
 * @param size The size of the memory block
 * @return The start address of the memory block
 */
void *malloc(const size_t size) {
    mem_header_t *mh = alloc_node(&heap_buddy, size_with_header(size));
    mh->ptr_validate = (void *)mh;
    mh->size = size;
    const auto ptr = (void *)((uintptr_t)mh + HEADER_SIZE);
    memset(ptr, 0, size);
    return ptr;
}

/**
 * alloc a block of memory that contains number of bytes,
 * size of the element is size.
 */
void *calloc(const size_t num, const size_t size) {
    void* ptr = malloc(size * num);
    memset(ptr, 0, size * num);
    return ptr;
}

/**
 * Release the memory block
 * @param ptr The start address of the memory block
 */
void free(void *ptr) {
    free_len(ptr, 0);
}

/**
 * Release the memory block
 * @param ptr The start address of the memory block
 * @param len The size of the memory block
 */
void free_len(void *ptr, size_t len) {
    // Free memory node
    const auto mh = (mem_header_t *)((uintptr_t)ptr - HEADER_SIZE);
    assert(mh->ptr_validate == (void *)mh);
    if(len == 0) {
        len = mh->size;
    } else {
        assert(len == mh->size);
    }
    // Add the released node to the buddy header.
    add_range(&heap_buddy, (uintptr_t)mh, len);
}
