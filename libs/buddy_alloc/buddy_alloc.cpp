#include <buddy_alloc.h>
#include <common.h>
#include <console.h>

/* Heap allocator, heap */
uint8_t heap[HEAP_SIZE] = {0};

/* Define BuddyLinked Node Type */
typedef struct _BuddyLinked BuddyLinked;
/* Define BuddyLinked Node Real Struct */
struct _BuddyLinked {
    BuddyLinked *next;
};

/* Static assert or Compiler assert */
static_assert(
    sizeof(BuddyLinked) == sizeof(intptr_t),
    "sizeof(BuddyLinkedAssert) Must be same as the sizeof(uintptr_t)");

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
    for(size_t l = 8; l < size; l <<= 1)
        index++;
    return index;
}

/**
 * Add a buddy node to the header list
 * @param index buddy header index
 * @param addr the memory block will be added
 * @return void
 */
inline void add_node(size_t index, uintptr_t addr) {
    BuddyLinked *node = (BuddyLinked *)addr;
    // if(buddy_header[index].next == nullptr) {
    //     node->next = nullptr;
    //     buddy_header[index].next = node;
    //     return;
    // }
    BuddyLinked *link = &buddy_header[index];
    while(link->next != nullptr && (uintptr_t)link->next < (uintptr_t)node) {
        link = link->next;
    }
    node->next = link->next;
    link->next = node;
}

/**
 * Add memory to buddy header list
 * @param addr The address of the memory block
 * @param size The size of the memory block
 */
void mem_add(uintptr_t addr, size_t size) {
    uintptr_t end = addr + size;
    // align to 8 bytes.
    addr = (addr + 7) & ~0x7;
    end &= ~0x7;
    // add to the buddy header list
    for(size_t index = 0; addr < end; index++) {
        size_t bit_and = 8 << index;
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
    auto index = buddy_header_index(size);
    // alloc buddy node
    auto node = buddy_header[index].next;
    // find the first available buddy node
    while(node == nullptr) {
        node = buddy_header[++index].next;
    }
    buddy_header[index].next = node->next;
    uintptr_t addr = (uintptr_t)node;
    size_t avaiable = 8 << index;

    // add the last bytes to the buddy.
    if(avaiable != size)
        mem_add(addr + size, avaiable - size);

    return (void *)addr;
}

/**
 * reimplement new
 * @param size The size of the memory block will be allocated
 * @return The address of the memory block
 */
void *operator new[](size_t size) {
    return malloc(size);
}

/**
 * Release the memory block
 * @param ptr The start address of the memory block
 * @param size The size of the memory block
 */
void free(void *ptr, size_t size) {
    auto index = buddy_header_index(size);
    // Ensure that ptr is aligned size.
    assert((uintptr_t)ptr % size == 0);
    // Free memory node
    // auto node = (BuddyLinked *)ptr;
    // node->next = buddy_header[index].next;
    // buddy_header[index].next = node;

    // // merge buddy node
    // while(index > 0) {
    //     index--;
    //     auto left = (BuddyLinked *)((uintptr_t)node - (8 << index));
    //     auto right = (BuddyLinked *)((uintptr_t)node + (8 << index));
    //     if(left->next == node && right->next == node) {
    //         node->next = nullptr;
    //         mem_add((uintptr_t)node, 8 << (index + 1));
    //     }
    // }
}

/* reimplement delete */
void operator delete(void *ptr, size_t size) {}
