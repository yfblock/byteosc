#include <buddy_alloc.h>
#include <common.h>
#include <console.h>
#include <string.h>
#define HEADER_SIZE sizeof(mem_header_t)

/* Heap allocator, heap */
uint8_t heap[HEAP_SIZE] = {0};

/* Define BuddyLinked Node Type */
typedef struct buddy_linked buddy_linked_t;
/* Define BuddyLinked Node Real Struct */
struct buddy_linked {
    buddy_linked_t *next;
};

typedef struct {
    /* Unit validate, equals the real raw pointer. */
    void *ptr_validate;
    /* Size of this memory unit. */
    size_t size;
} mem_header_t;

/**
 * Get the memory size with memory header.
 */
static inline size_t size_with_header(const size_t size) {
    return size + HEADER_SIZE;
}

/* Static assert or Compiler assert */
// static_assert(
//     sizeof(BuddyLinked) == sizeof(intptr_t),
//     "sizeof(BuddyLinkedAssert) Must be same as the sizeof(uintptr_t)");

/* Support 8 << MAX_BUDDY_HEADER_BITS SIZE, */
static buddy_linked_t buddy_header[MAX_BUDDY_HEADER_BITS] = {nullptr};

/**
 * Get the index level through the allocation size.
 * @param size The size of the allocation
 * @return The index level, 0 is 8 bytes, 1 is 16 bytes...
 */
inline size_t buddy_header_index(const size_t size) {
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
void add_node(const size_t index, const uintptr_t addr) {
    const auto node = (buddy_linked_t *)addr;
    buddy_linked_t *link = &buddy_header[index];
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
void mem_add(uintptr_t addr, const size_t size) {
    uintptr_t end = addr + size;
    // align to 8 bytes.
    addr = (addr + (MIN_UNIT_SIZE - 1)) & ~(MIN_UNIT_SIZE - 1);
    end &= ~(MIN_UNIT_SIZE - 1);
    // add to the buddy header list
    for(size_t index = 0; addr < end; index++) {
        const size_t bit_and = MIN_UNIT_SIZE << index;
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
    buddy_linked_t* node = buddy_header[index].next;
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

    mem_header_t *mh = (mem_header_t *)addr;
    mh->ptr_validate = (void *)mh;
    mh->size = size;

    return (void *)(addr + HEADER_SIZE);
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
    add_node(buddy_header_index(mh->size), (uintptr_t)mh);
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
    buddy_linked_t *node = buddy_header[idx].next;
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

static void* page_header[MAX_BUDDY_HEADER_BITS] = {nullptr};

/**
 * Add the pages to frame buddy allocator.
 * @param addr_s the start address of the added range.
 * @param addr_e the end address of the added range.
 */
void add_frame_pages(uintptr_t addr_s, uintptr_t addr_e) {
    uintptr_t page_s = addr_s >> 12;
    uintptr_t page_e = addr_e >> 12;


}

/**
 * Allocate count pages memory aligned with PAGE_SIZE
 * @param count the count page number will be allocated
 * @return the pointer of the allocated memory
 */
void *kalloc(size_t count) {
    warn("todo: Allocate page aligned memory");
    return nullptr;
}

/**
 * Release a physical pages.
 * @param ptr The pointer of the page that will be released.
 * @param count Release count pages.
 */
void kfree(void *ptr, size_t count) {
    
}
