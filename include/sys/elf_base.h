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

enum Ph_Types {
    PT_NULL = 0,
    PT_LOAD,
    PT_DYNAMIC,
};

enum Ph_Flags {
    PF_X = 0x1,
    PF_W = 0x2,
    PF_R = 0x4
};
