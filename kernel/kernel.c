#include "elf_parser.h"


#include <arch.h>
#include <buddy_alloc.h>
#include <common.h>
#include <console.h>
#include <driver.h>
#include <percpu.h>
#include <smoldtb.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ext4.h>
void test_heap() {
    // test heap alloc 1
    char *test_alloc = malloc(sizeof(char) * 0x201);

    assert(test_alloc != NULL);
    memset(test_alloc, 3, sizeof(char) * 0x201);
    free(test_alloc);

    // test heap alloc 2
    char *test_alloc1 = malloc(sizeof(char) * 0x200);

    assert(test_alloc1 != NULL);

    memset(test_alloc1, 4, sizeof(char) * 0x200);
    free(test_alloc1);

    int *test_arr = talloc(10, int);
    free(test_arr);

    int *test_arr1 = talloc(10, int);
    free(test_arr1);
}

// percpu int test_int = 0;
percpu_define(int, test_int, 4000);

/// CTOR Test
CTOR void test_ctor() {
    debug("constructor: %s", __PRETTY_FUNCTION__);
}

void cmain(size_t hart_id, uintptr_t dtb) {
    // Print Banner
    printf(NEWLINE(RAW("(  ____        _        ____   _____  )")));
    printf(NEWLINE(RAW("( |  _ \      | |      / __ \ / ____| )")));
    printf(NEWLINE(RAW("( | |_) |_   _| |_ ___| |  | | (___   )")));
    printf(NEWLINE(RAW("( |  _ <| | | | __/ _ \ |  | |\___ \  )")));
    printf(NEWLINE(RAW("( | |_) | |_| | ||  __/ |__| |____) | )")));
    printf(NEWLINE(RAW("( |____/ \__, |\__\___|\____/|_____/  )")));
    printf(NEWLINE(RAW("(         __/ |         C Version     )")));
    printf(NEWLINE(RAW("(        |___/                        )")));
    printf("\n");

    debug("Hart ID: %d", hart_id);
    debug("dtb address: 0x%x", dtb);

    // Ensure boot hart is 0
    assert(hart_id == 0);
    // Ensure has dtb
    assert(dtb != 0);

    info("percpu pointer: 0x%x", percpu_pointer());

    info("DTB ADDR: 0x%x", dtb);
    info("C Standard: %dL", __STDC_VERSION__);
    info("Size of boot stack: 0x%x", boot_stack_size());

    debug("add heap 0x%x - 0x%x", &heap, HEAP_SIZE);

    // test Heap allocator
    test_heap();

    debug("init device tree");

    dtb_node_t *mnode = dtb_find("/memory");
    if(mnode != NULL) {
        print_node(mnode, 0);
        dtb_prop_t *prop = dtb_find_prop(mnode, "reg");
        dtb_pair mrange = {.a = 2, .b = 2};
        dtb_read_prop_pairs(prop, mrange, &mrange);
        printf("Memory Range: 0x%lx - 0x%lx\n", mrange.a, mrange.a + mrange.b);
        // Add Memory Range to Frame Allocator.
        add_frame_range(mrange.a, mrange.a + mrange.b);
    }

    // iterator the init_array.
    for_each_init(func) {
        debug("Initial constructors");
        (*func)();
    }

    probe_dtb(dtb_find("/"));

    int r = ext4_mount("ext4_fs", "/", true);
    if(r != EOK)
        debug("ext4 mount ret: %d", r);

    r = ext4_recover("/");
    if(r != EOK)
        debug("ext4 recover ret: %d", r);

    r = ext4_journal_start("/");
    if(r != EOK)
        debug("ext4 journal start ret: %d", r);

    ext4_cache_write_back("/", 1);

    ext4_file ext4_file;
    ext4_fopen(&ext4_file, "/a.out", "r+");
    elf64_file_t elf_file;
    read_elf_file(&ext4_file, &elf_file);

    dump_heap();

    void test_fs();
    test_fs();

    log(LOG_LEVEL_WARNING, "Hello %d %s!\n", 35, "World");
}
