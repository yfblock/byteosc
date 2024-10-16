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
#include <ext4_fs.h>
#include <elf_parser.h>

void test_heap() {
    // test heap alloc 1
    char *test_alloc = (char *)malloc(sizeof(char) * 0x201);

    assert(test_alloc != NULL);
    memset(test_alloc, 3, sizeof(char) * 0x201);
    free(test_alloc);

    // test heap alloc 2
    char *test_alloc1 = (char *)malloc(sizeof(char) * 0x200);

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
    printf(NEWLINE(R"(  ____        _        ____   _____  )"));
    printf(NEWLINE(R"( |  _ \      | |      / __ \ / ____| )"));
    printf(NEWLINE(R"( | |_) |_   _| |_ ___| |  | | (___   )"));
    printf(NEWLINE(R"( |  _ <| | | | __/ _ \ |  | |\___ \  )"));
    printf(NEWLINE(R"( | |_) | |_| | ||  __/ |__| |____) | )"));
    printf(NEWLINE(R"( |____/ \__, |\__\___|\____/|_____/  )"));
    printf(NEWLINE(R"(         __/ |         C Version     )"));
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

    // Test EXT4, This is a temporary code testcases.
    struct ext4_fs fs;
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

    const char* path = "/";
    char sss[255];
    ext4_dir d;

    printf("ls %s\n", path);

    r = ext4_dir_open(&d, path);
    if(r != EOK)
        debug("ext4 open ret: %d", r);

    const ext4_direntry *de = ext4_dir_entry_next(&d);
    while (de) {
        memcpy(sss, de->name, de->name_length);
        sss[de->name_length] = 0;
        printf("  [%d] %d %s\n", de->inode_type, de->entry_length, sss);
        de = ext4_dir_entry_next(&d);
    }
    ext4_dir_close(&d);

    ext4_file *file = talloc(1, ext4_file);
    r = ext4_fopen(file, "/1.log", "r");
    if(r != EOK)
        debug("open file failed %d", r);

    char *buffer = talloc(0x1001, char);
    size_t size = 0;
    r = ext4_fread(file, buffer, 0x1ff, &size);
    if(r != EOK)
        debug("read file %d  size=%x", r, size);

    printf(buffer);
    ext4_fclose(file);

    free(buffer);
    free(file);

    ext4_file *elf_file = talloc(1, ext4_file);
    r = ext4_fopen(elf_file, "/a.out", "r+");
    if(r != EOK)
        debug("open a.out file failed %d", r);
    debug("elf file size: %d", elf_file->fsize);
    elf64_header_t *elf_header = talloc(0x1000, char);
    r = ext4_fread(elf_file, elf_header, 0x1000, &size);

    debug("elf fread size: %d", size);

    if(!(elf_header->magic[0] == 0x7f &&
        strncmp("ELF", elf_header->magic + 1, 3) == 0)) {
        debug("This is not a valid elf header");
    }

    debug("elf header: entry offset: %x", elf_header->entry_off);


    debug("section offset: %x", elf_header->sht_off);
    ext4_fseek(elf_file, elf_header->pht_off, 0);
    char *buf = malloc(elf_header->pht_e_sz * elf_header->pht_e_n);
    ext4_fread(elf_file, buf, elf_header->pht_e_sz * elf_header->pht_e_n, nullptr);

    for(int i = 0;i < elf_header->pht_e_n;i++) {
        elf64_ph_t *ph = (elf64_ph_t *) (buf + i * elf_header->pht_e_sz);
        debug("program header: %x  TYPE: %x", ph->flags, ph->type);
        debug("ph vaddr: %x paddr: %x memsz: %x", ph->p_vaddr, ph->p_paddr, ph->p_memsz);
    }

    free(buf);
    ext4_fseek(elf_file, elf_header->sht_off, 0);
    buf = malloc(elf_header->sht_e_sz * elf_header->sht_e_n);
    ext4_fread(elf_file, buf, elf_header->sht_e_sz * elf_header->sht_e_n, nullptr);

    for(int i = 0;i < elf_header->sht_e_n;i++) {
        elf64_sh_t *sh = (elf64_sh_t *) (buf + i * elf_header->sht_e_sz);
        debug("section header: %x  TYPE: %x", sh->sh_flags, sh->sh_type);
        debug("sh vaddr: %08x - %08x ", sh->sh_addr, sh->sh_addr + sh->sh_size);
    }

    log(LOG_LEVEL_WARNING, "Hello %d %s!\n", 35, "World");
}
