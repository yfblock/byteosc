#pragma once
#include <stdint.h>
#include <ext4.h>

#define ELF_ISA_NO_SPECIFIC 	0x00
#define ELF_ISA_SPARC 	        0x02
#define ELF_ISA_x86 	        0x03
#define ELF_ISA_MIPS 	        0x08
#define ELF_ISA_PowerPC 	    0x14
#define ELF_ISA_ARM 	        0x28
#define ELF_ISA_SuperH 	        0x2A
#define ELF_ISA_IA_64 	        0x32
#define ELF_ISA_x86_64 	        0x3E
#define ELF_ISA_AArch64 	    0xB7
#define ELF_ISA_RISC_V 	        0xF3

typedef struct {
    char magic[4];
    uint8_t bit_width;
    uint8_t endian;
    uint8_t version;
    uint8_t abi;
    uint64_t _padding0;
    uint16_t type;
    uint16_t ia; // Instruction Set
    uint32_t elf_version;
    uint64_t entry_off;
    uint64_t pht_off; // Program Header Table Offset
    uint64_t sht_off; // Section Header Table Offset
    uint32_t flags;
    uint16_t header_sz;
    uint16_t pht_e_sz;  // size of entry in the program header table
    uint16_t pht_e_n; // entry nums
    uint16_t sht_e_sz; // size of entry in the section header table
    uint16_t sht_e_n;
    uint16_t section_idx;
} elf64_header_t;

#define ELF_PH_EXEC  1
#define ELF_PH_WRITE 2
#define ELF_PH_READ  4

#define ELF_PH_TYPE_NONE 0
#define ELF_PH_TYPE_LOAD 1
#define ELF_PH_TYPE_DYNAMIC 2
#define ELF_PH_TYPE_INTERP 3
#define ELF_PH_TYPE_NOTE 4

typedef struct {
    uint32_t type;
    uint32_t flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_alignment;
} elf64_ph_t;

#define ELF64_SHT_NULL 0
#define ELF64_SHT_PROGBITS 1
#define ELF64_SHT_SYMTAB 2
#define ELF64_SHT_STRTAB 3
#define ELF64_SHT_RELA 4
#define ELF64_SHT_HASH 5
#define ELF64_SHT_DYNAMIC 6
#define ELF64_SHT_NOTE 7
#define ELF64_SHT_NOBITS 8
#define ELF64_SHT_REL 9
#define ELF64_SHT_SHLIB 10

#define ELF64_SHF_WRITE 1
#define ELF64_SHF_ALLOC 2
#define ELF64_SHF_EXEC 4
#define ELF64_SHF_MERGE 0x10
#define ELF64_SHF_STRING 0x20
#define ELF64_SHF_INFO_LINK 0x40
#define ELF64_SHF_INFO_ORDER 0x80
#define ELF64_SHF_OS_NOCONFIRM 0x100
#define ELF64_SHF_GROUP 0x200
#define ELF64_SHF_TLS 0x400
#define ELF64_SHF_MASKOS 0x0ff00000
#define ELF64_SHF_MASKPROC 0xf0000000

typedef struct {
    uint32_t	sh_name;
    uint32_t	sh_type;
    uint64_t	sh_flags;
    uint64_t	sh_addr;
    uint64_t	sh_offset;
    uint64_t	sh_size;
    // Abot Link And info
    // https://docs.oracle.com/cd/E19620-01/805-4694/6j4enatcv/index.html#elf-20463
    uint32_t	sh_link;
    uint32_t	sh_info;
    uint64_t	sh_addralign;
    uint64_t	sh_entsize;
} elf64_sh_t;

typedef struct {
    uint32_t	st_name;
    uint8_t	    st_info;
    uint8_t	    st_other;
    uint16_t	st_shndx;
    uint64_t	st_value;
    uint64_t	st_size;
} Elf64_Sym;

typedef struct {
    elf64_header_t *header;
    // Program Header
    elf64_ph_t *ph;
    // Section Header
    elf64_sh_t *sh;
    // string table
    char *strtab;
    // shstrtab
    char *shstrtab;
} elf64_file_t;

bool check_elf64(const char *ptr);
int read_elf_file(ext4_file *f, elf64_file_t *elf);
