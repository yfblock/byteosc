#include <arch.h>
#include <buddy_alloc.h>
#include <common.h>
#include <console.h>
#include <percpu.h>
#include <smoldtb.h>
#include <stddef.h>
#include <string.h>

/**
 * Print DTB Nodes for Debugging.
 */
void print_node(dtb_node *node, size_t indent) {
    const size_t indent_scale = 2;
    if(node == NULL)
        return;

    char indent_buff[indent + 1];
    for(size_t i = 0; i < indent; i++)
        indent_buff[i] = ' ';
    indent_buff[indent] = 0;

    dtb_node_stat stat;
    dtb_stat_node(node, &stat);
    printf("%s[+] %s: %d siblings, %d children, %x properties.\r\n",
           indent_buff, stat.name, stat.sibling_count, stat.child_count,
           stat.prop_count);

    for(size_t i = 0; i < stat.prop_count; i++) {
        dtb_prop *prop = dtb_get_prop(node, i);
        if(prop == NULL)
            break;
        // NOTE: DO NOT DO THIS! This is a hack for testing purposes for I can
        // make print pretty trees and check all properties are read correctly.
        // There's a reason these structs are opaque to calling code, and their
        // underlying definitions can change at any time.
        printf("%s  | %s: %x", indent_buff, prop->name, prop->first_cell);
        // for(int i = 0;i < prop->length;i+=sizeof(*prop->first_cell)) {
        //     printf(" %x", *(prop->first_cell + i/sizeof(*prop->first_cell)));
        // }
        printf("\r\n");
    }

    dtb_node *child = dtb_get_child(node);
    while(child != NULL) {
        print_node(child, indent + indent_scale);
        child = dtb_get_sibling(child);
    }
}

void test_heap() {

    // test heap alloc 1
    char *test_alloc = (char *)malloc(sizeof(char) * 0x201);

    assert(test_alloc != NULL);

    memset(test_alloc, 3, sizeof(char) * 0x201);

    // test heap alloc 2
    char *test_alloc1 = (char *)malloc(sizeof(char) * 0x200);

    assert(test_alloc != NULL);

    memset(test_alloc1, 4, sizeof(char) * 0x200);
}

// percpu int test_int = 0;
percpu_define(int, test_int, 4000);

/// CTOR Test
CTOR void test_ctor() {
    debug("constructor: %s", __PRETTY_FUNCTION__);
}

void cmain(size_t hart_id, uintptr_t dtb) {
    // Print Banner
    printf(NEWLINE(R"(  ____        _        ____   _____  )"));
    printf(NEWLINE(R"( |  _ \      | |      / __ \ / ____| )"));
    printf(NEWLINE(R"( | |_) |_   _| |_ ___| |  | | (___   )"));
    printf(NEWLINE(R"( |  _ <| | | | __/ _ \ |  | |\___ \  )"));
    printf(NEWLINE(R"( | |_) | |_| | ||  __/ |__| |____) | )"));
    printf(NEWLINE(R"( |____/ \__, |\__\___|\____/|_____/  )"));
    printf(NEWLINE(R"(         __/ |         C++ Version   )"));
    printf(NEWLINE(R"(        |___/                        )"));
    printf("\n");

    debug("Hart ID: %d", hart_id);
    debug("dtb address: 0x%x", dtb);

    // Ensure boot hart is 0
    assert(hart_id == 0);
    // Ensure has dtb
    assert(dtb != 0);

    info("percpu pointer: 0x%x", percpu_pointer());

    info("DTB ADDR: 0x%x", dtb);
    // info("C++ Standard: %s", __STDC_VERSION__);
    info("Size of boot stack: 0x%x", boot_stack_size());
    // Add heap memory to allocator
    mem_add((uintptr_t)&heap, HEAP_SIZE);
    debug("add heap 0x%x - 0x%x", &heap, HEAP_SIZE);

    // test Heap allocator
    test_heap();
    debug("init device tree");
    dtb_ops dtb_ops_impl = {.malloc = malloc,
                          .free = free_len,
                          .on_error = puts};

    dtb_init(dtb, dtb_ops_impl);

    // dtb_node *root = dtb_find("/");
    // printf("root ptr: 0x%x\n", root);

    // while(root != NULL) {
    //     print_node(root, 0);
    //     root = dtb_get_sibling(root);
    // }
    debug("print memory info from device tree");
    dtb_node *mnode = dtb_find("/memory");
    if(mnode != NULL) {
        print_node(mnode, 0);
        dtb_prop *prop = dtb_find_prop(mnode, "reg");
        dtb_pair mrange = {.a = 2, .b = 2};
        dtb_read_prop_pairs(prop, mrange, &mrange);
        printf("Memory Range: 0x%x - 0x%x\n", mrange.a, mrange.a + mrange.b);
        // TOOD: Add Memory Range to Frame Allocator.
    }

    // iterator the init_array.
    for_each_init(func) {
        debug("Initial constructors");
        (*func)();
    }

    void uart_drv_putchar(char c);
    uart_drv_putchar('3');

    int *test_arr = malloc(sizeof(int) * 10);
    debug("test arr addr: 0x%x", test_arr);
    free(test_arr);


    int *test_arr1 = malloc(sizeof(int) * 10);
    debug("test arr addr: 0x%x", test_arr1);
    assert(test_arr == test_arr1);
    free(test_arr1);

    log(LOG_LEVEL_WARNING, "Hello %d %s!\n", 35, "World");
}
