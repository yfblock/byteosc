#include <arch.h>
#include <buddy_alloc.h>
#include <common.h>
#include <console.h>
#include <percpu.h>
#include <smoldtb.h>
#include <string.h>

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
        printf("%s  | %s: %x\r\n", indent_buff, prop->name, prop->first_cell);
    }

    dtb_node *child = dtb_get_child(node);
    while(child != NULL) {
        print_node(child, indent + indent_scale);
        child = dtb_get_sibling(child);
    }
}

void test_heap() {

    // test heap alloc 1
    char *test_alloc = new char[0x201];

    assert(test_alloc != nullptr);

    memset(test_alloc, 3, sizeof(test_alloc));
    debug("test ptr: 0x%x   size: 0x%x", test_alloc, sizeof(test_alloc));
    for(int i = 0; i < 10; i++) {
        debug("root %x: %x", i, test_alloc[i]);
    }

    // test heap alloc 2
    char *test_alloc1 = new char[0x200];

    assert(test_alloc != nullptr);
    assert((uintptr_t)test_alloc1 % 0x200 == 0);

    memset(test_alloc1, 4, sizeof(test_alloc1));
    debug("test ptr: 0x%x   size: 0x%x", test_alloc1, sizeof(test_alloc1));
    for(int i = 0; i < 10; i++) {
        debug("root %d: %d", i, test_alloc1[i]);
    }
}

// percpu int test_int = 0;
percpu_define(int, test_int, 4000);

int main(size_t hart_id, uintptr_t dtb) {
    // Print Banner
    printf(CONCAT(R"(  ____        _        ____   _____  )", "\n"));
    printf(CONCAT(R"( |  _ \      | |      / __ \ / ____| )", "\n"));
    printf(CONCAT(R"( | |_) |_   _| |_ ___| |  | | (___   )", "\n"));
    printf(CONCAT(R"( |  _ <| | | | __/ _ \ |  | |\___ \  )", "\n"));
    printf(CONCAT(R"( | |_) | |_| | ||  __/ |__| |____) | )", "\n"));
    printf(CONCAT(R"( |____/ \__, |\__\___|\____/|_____/  )", "\n"));
    printf(CONCAT(R"(         __/ |         C++ Version   )", "\n"));
    printf(CONCAT(R"(        |___/                        )", "\n"));
    printf("\n");

    // Ensure boot hart is 0
    assert(hart_id == 0);
    // Ensure has dtb
    assert(dtb != 0);

    info("percpu pointer: 0x%x", percpu_pointer());

    info("DTB ADDR: 0x%x", dtb);
    info("C++ Standard: %s", STR(__cplusplus));
    info("Size of boot stack: 0x%x", boot_stack_size());
    // Add heap memory to allocator
    mem_add((uintptr_t)&heap, HEAP_SIZE);
    debug("add heap 0x%x 0x%x", &heap, HEAP_SIZE);

    // test Heap allocator
    test_heap();

    char str[50];
    memset(str, '$', 10);
    puts((char *)str);
    puts((char *)"root\n");

    dtb_init(dtb, dtb_ops{.on_error = [](const char *str) -> void {
                 puts((char *)str);
             }});

    dtb_node *root = dtb_find("/");
    printf("root ptr: 0x%x\n", root);

    while(root != NULL) {
        // print_node(root, 0);
        root = dtb_get_sibling(root);
    }
    log(LOG_LEVEL_WARNING, "Hello %d %s!\n", 35, "World");
}
