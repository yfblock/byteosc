#include "elf_parser.h"
#include "stdlib.h"
#include <console.h>
#include <ext4.h>

#include <string.h>

/**
 * Return true if it is elf64 file. Otherwise false.
 * @param ptr the pointer to the elf file header.
 * @return if it is a valid elf64 file.
 */
bool check_elf64(const char *ptr) {
    const auto header = (elf64_header_t *)ptr;
    if(header->magic[0] != 0x7f)
        return false;
    if(strncmp("ELF", header->magic + 1, 3) != 0)
        return false;
    return true;
}

// elf64_ph_t *read_elf64_ph(const elf64_header_t *header) {
//     ext4_fseek(elf_file, header->pht_off, 0);
//     char *buf = malloc(header->pht_e_sz * header->pht_e_n);
//     ext4_fread(elf_file, buf, header->pht_e_sz * header->pht_e_n, nullptr);
//
// }

elf64_sh_t *read_elf64_sh(const elf64_header_t *header) {
    return nullptr;
}

int read_elf_file(ext4_file *f, elf64_file_t *elf) {
    int             r      = 0;
    elf64_header_t *header = talloc(1, elf64_header_t);
    debug("header addr: %x", header);
    ext4_fseek(f, 0, SEEK_SET);
    r = ext4_fread(f, (void *)header, sizeof(elf64_header_t), nullptr);
    if(r != EOK)
        return r;

    elf->header = header;

    // Read Program Header
    ext4_fseek(f, (int64_t)header->pht_off, 0);
    elf->ph = malloc(header->pht_e_sz * header->pht_e_n);
    debug("elf ph addr: %x", elf->sh);
    ext4_fread(f, elf->ph, header->pht_e_sz * header->pht_e_n, nullptr);
    debug("elf ph addr: %x", elf->sh);

    // Read Program Header

    ext4_fseek(f, (int64_t)header->sht_off, 0);
    elf->sh = malloc(header->sht_e_sz * header->sht_e_n);
    debug("elf ph addr: %x", elf->sh);
    ext4_fread(f, elf->sh, header->sht_e_sz * header->sht_e_n, nullptr);
    debug("elf sh addr: %x", elf->sh);
    return 0;
}
