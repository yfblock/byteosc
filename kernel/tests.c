#include <buddy_alloc.h>
#include <console.h>
#include <elf_parser.h>
#include <ext4.h>
#include <ext4_fs.h>
#include <stdio.h>
#include <string.h>

void test_fs() {
    // Test EXT4, This is a temporary code testcases.
    struct ext4_fs fs;
    int            r    = 0;

    const char    *path = "/";
    char           sss[255];
    ext4_dir       d;

    printf("ls %s\n", path);

    r = ext4_dir_open(&d, path);
    if(r != EOK)
        debug("ext4 open ret: %d", r);

    const ext4_direntry *de = ext4_dir_entry_next(&d);
    while(de) {
        memcpy(sss, de->name, de->name_length);
        sss[de->name_length] = 0;
        printf("  [%d] %d %s\n", de->inode_type, de->entry_length, sss);
        de = ext4_dir_entry_next(&d);
    }
    ext4_dir_close(&d);

    // ========= Test Read File ===========
    ext4_file *file = talloc(1, ext4_file);
    r               = ext4_fopen(file, "/1.log", "r");
    if(r != EOK)
        debug("open file failed %d", r);

    char  *buffer = talloc(0x1001, char);
    size_t size   = 0;
    r             = ext4_fread(file, buffer, 0x1ff, &size);
    if(r != EOK)
        debug("read file %d  size=%x", r, size);

    printf(buffer);
    ext4_fclose(file);

    free(buffer);
    free(file);
    // ========= Test Read File ===========

    // ========= Test Read ELF ============
    ext4_file *elf_file = talloc(1, ext4_file);
    r                   = ext4_fopen(elf_file, "/a.out", "r+");
    if(r != EOK)
        debug("open a.out file failed %d", r);
    debug("elf file size: %d", elf_file->fsize);

    debug("elf file content");
    elf64_file_t *elf;
    read_elf_file(elf_file, elf);

    debug("sh addr: %x", elf->sh);
    for(int i = 0; i < elf->header->sht_e_n; i++) {
        elf64_sh_t *sh =
            (elf64_sh_t *)((char *)elf->sh + i * elf->header->sht_e_sz);
        debug("vaddr sh: %x", sh);
        debug("section header: %x  TYPE: %x", sh->sh_flags, sh->sh_type);
        debug("sh vaddr: %08x - %08x ", sh->sh_addr, sh->sh_addr + sh->sh_size);
    }
}
