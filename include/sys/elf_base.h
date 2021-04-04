#pragma once

#include <types/base.h>

#define ELF_NIDENT  16
#define ELF_MAGIC_0 0x7F
#define ELF_MAGIC_1 'E'
#define ELF_MAGIC_2 'L'
#define ELF_MAGIC_3 'F'

#define ELFCLASS32 1

#define ELFDATA2LSB 1

/* ISA codes  */
#define EM_386 3

typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_Sword;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;

enum Elf_Ident_Index {
    EIDX_MAGIC0,
    EIDX_MAGIC1,
    EIDX_MAGIC2,
    EIDX_MAGIC3,
    EIDX_CLASS, // Architecture (32/64)
    EIDX_BYTE_ORDER, // Byte Order
    EIDX_VERSION, // ELF Version
    EIDX_OSABI, // OS Specific
    EIDX_ABIVERSION, // OS Specific
    EIDX_PAD
};

enum Elf_Type {
    ET_NONE = 0,
    ET_REL  = 1,
    ET_EXEC = 2
};

typedef struct {
    unsigned char e_ident[ELF_NIDENT];
    Elf32_Half    e_type;
    Elf32_Half    e_machine;
    Elf32_Word    e_version;
    Elf32_Addr    e_entry; /* Entry point */
    Elf32_Off     e_phoff;
    Elf32_Off     e_shoff;
    Elf32_Word    e_flags;
    Elf32_Half    e_ehsize;
    Elf32_Half    e_phentsize;
    Elf32_Half    e_phnum;
    Elf32_Half    e_shentsize;
    Elf32_Half    e_shnum;
    Elf32_Half    e_shstrndx;
} Elf32_Hdr;

/* Section header */
enum ShT_Types {
    SHT_NULL     = 0, // Null section
    SHT_PROGBITS = 1, // Program information
    SHT_SYMTAB   = 2, // Symbol table
    SHT_STRTAB   = 3, // String table
    SHT_RELA     = 4, // Relocation (w/ addend)
    SHT_NOBITS   = 8, // Not present in file
    SHT_REL      = 9, // Relocation (no addend)
};

enum ShT_Attributes {
    SHF_WRITE = 0x01, // Writable section
    SHF_ALLOC = 0x02 // Exists in memory
};

typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off  sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;
