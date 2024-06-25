#include <buddy_alloc.h>

typedef struct _BuddyLinked
{
    BuddyLinked *next;
}BuddyLinked;

/// Support 8 << MAX_BUDDY_HEADER_BITS SIZE,
static BuddyLinked buddy_header[MAX_BUDDY_HEADER_BITS] = { 0 };

__always_inline size_t buddy_header_index(size_t size) {
    size_t index = 0;
    // End when l >= size, so l is the min size available when satisfying the size limit
    for(size_t l = 8;l < size;l <<= 2) index++;
    return index;
}

/// @brief Add a buddy node to the header list
/// @param index buddy header index
/// @param addr the memory block will be added
/// @return void
__always_inline void add_node(size_t index, uintptr_t addr) {
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

/// @brief Add memory to buddy header list
/// @param addr The address of the memory block
/// @param size The size of the memory block
void mem_add(uintptr_t addr, size_t size) {
    uintptr_t end = addr + size;
    // align to 8 bytes
    addr &= ~0x7;
    end &= ~0x7;
    // add to the buddy header list
    for(size_t index = 0;addr < end;index++) {
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

// reimplement new
void* operator new[](size_t size) {
    auto index = buddy_header_index(size);
    size_t avaiable = 2 << (3 + index);

    // alloc buddy node
    // TODO: alloc from upper level when the current level buddy is empty
    auto node = buddy_header[index].next;
    buddy_header[index].next = node->next;
    uintptr_t addr = (uintptr_t)node;

    // add the last bytes to the buddy.
    if(avaiable != size) 
        mem_add(addr + size, avaiable - size);

    return (void *)addr;
}

// reimplement delete
void operator delete(void *ptr, size_t size) {
    
}
