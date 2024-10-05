#include <buddy_alloc.h>
#include <common.h>
#include <console.h>
#define HEADER_SIZE sizeof(MemoryHeader)

/* Heap allocator, heap */
uint8_t heap[HEAP_SIZE] = {0};

/* Define BuddyLinked Node Type */
typedef struct _BuddyLinked BuddyLinked;
/* Define BuddyLinked Node Real Struct */
struct _BuddyLinked {
    BuddyLinked *next;
};

typedef struct {
    // Ensure that ptr equals to the real raw pointer.
    void *ptr_verilate;
    /// @brief Memory size.
    size_t size;
} MemoryHeader;

/**
 * Get the memory size with memory header.
 */
static inline size_t size_with_header(size_t size) {
    return size + HEADER_SIZE;
}

/* Static assert or Compiler assert */
// static_assert(
//     sizeof(BuddyLinked) == sizeof(intptr_t),
//     "sizeof(BuddyLinkedAssert) Must be same as the sizeof(uintptr_t)");

/* Support 8 << MAX_BUDDY_HEADER_BITS SIZE, */
static BuddyLinked buddy_header[MAX_BUDDY_HEADER_BITS] = {0};

/**
 * Get the index level through the allocation size.
 * @param size The size of the allocation
 * @return The index level, 0 is 8 bytes, 1 is 16 bytes...
 */
inline size_t buddy_header_index(size_t size) {
    size_t index = 0;
    // End when l >= size, so l is the min size available when satisfying the
    // size limit
    for(size_t l = MIN_UNIT_SIZE; l < size; l <<= 1)
        index++;
    return index;
}

/**
 * Add a buddy node to the header list
 * @param index buddy header index
 * @param addr the memory block will be added
 * @return void
 */
void add_node(size_t index, uintptr_t addr) {
    BuddyLinked *node = (BuddyLinked *)addr;
    BuddyLinked *link = &buddy_header[index];
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
void mem_add(uintptr_t addr, size_t size) {
    uintptr_t end = addr + size;
    // align to 8 bytes.
    addr = (addr + (MIN_UNIT_SIZE - 1)) & ~(MIN_UNIT_SIZE - 1);
    end &= ~(MIN_UNIT_SIZE - 1);
    // add to the buddy header list
    for(size_t index = 0; addr < end; index++) {
        size_t bit_and = MIN_UNIT_SIZE << index;
        if((addr & bit_and) != 0) {
            add_node(index, addr);
            addr += bit_and;
        }
        if((end & bit_and) != 0) {
            end -= bit_and;
            add_node(index, end);
        }
    }
}

/**
 * alloc size of a memory block, aligned to 8 bytes
 * @param size The size of the memory block
 * @return The start address of the memory block
 */
void *malloc(size_t size) {
    size = size_with_header(size);
    size_t index = buddy_header_index(size);
    // alloc buddy node
    BuddyLinked* node = buddy_header[index].next;
    // find the first available buddy node
    while(node == NULL) {
        node = buddy_header[++index].next;
    }
    buddy_header[index].next = node->next;
    uintptr_t addr = (uintptr_t)node;
    size_t avaiable = 8 << index;

    // add the last bytes to the buddy.
    if(avaiable != size)
        mem_add(addr + size, avaiable - size);

    MemoryHeader *mh = (MemoryHeader *)addr;
    mh->ptr_verilate = (void *)mh;
    mh->size = size;

    debug("alloc addr: %x  len: %x", addr, size);
    return (void *)(addr + HEADER_SIZE);
}

/**
 * Release the memory block
 * @param ptr The start address of the memory block
 * @param size The size of the memory block
 */
void free(void *ptr) {
    free_len(ptr, 0);
}

/**
 * Release the memory block
 * @param ptr The start address of the memory block
 * @param size The size of the memory block
 */
void free_len(void *ptr, size_t len) {
    debug("free ptr: %x size: %x", ptr, len);
    // Free memory node
    MemoryHeader* mh = (MemoryHeader *)((uintptr_t)ptr - HEADER_SIZE);
    assert(mh->ptr_verilate = (void *)mh);
    if(len == 0) {
        len = mh->size;
    } else {
        assert(len == mh->size);
    }
    // Index of memory level.
    size_t index = buddy_header_index(mh->size);
    add_node(index, (uintptr_t)mh);
}

/* Support 8 << MAX_BUDDY_HEADER_BITS SIZE, */
// static BuddyLinked buddy_header[MAX_BUDDY_HEADER_BITS] = {0};

void *alloc_node(size_t size) {
    size_t node_size = MIN_UNIT_SIZE;
    int idx = 0;
    // Find Proper Buddy Node IDX
    for(; idx < MAX_BUDDY_HEADER_BITS; idx++) {
        if(node_size >= size && buddy_header[idx].next != NULL) {
            break;
        }
        node_size <<= 2;
    }
    // Return if not available node.
    if(idx == MAX_BUDDY_HEADER_BITS) {
        return NULL;
    }
    // Remove node from header.
    BuddyLinked *node = buddy_header[idx].next;
    buddy_header[idx].next = node->next;

    // assign the memory.
    size = ALIGN_BITS(size, 3);
    uintptr_t m_ptr = (uintptr_t)node;

    // store the last memory to the buddy header.
    uintptr_t last_ptr = m_ptr + size;
    uintptr_t last_size = (MIN_UNIT_SIZE << idx) - size;
    mem_add(last_ptr, last_size);

    return (void *)m_ptr;
}
