#pragma once

#include "elf_base.h"

typedef uint32_t Elf_Word;
typedef int32_t  Elf_Sword;
typedef uint16_t Elf_Half;
typedef uint32_t Elf_Addr;
typedef uint32_t Elf_Off;

typedef struct {
    unsigned char e_ident[ELF_NIDENT];
    Elf_Half      e_type;
    Elf_Half      e_machine;
    Elf_Word      e_version;
    Elf_Addr      e_entry;
    Elf_Off       e_phoff;
    Elf_Off       e_shoff;
    Elf_Word      e_flags;
    Elf_Half      e_ehsize;
    Elf_Half      e_phentsize;
    Elf_Half      e_phnum;
    Elf_Half      e_shentsize;
    Elf_Half      e_shnum;
    Elf_Half      e_shstrndx;
} Elf_Hdr;

typedef struct {
    Elf_Word sh_name;
    Elf_Word sh_type;
    Elf_Word sh_flags;
    Elf_Addr sh_addr;
    Elf_Off  sh_offset;
    Elf_Word sh_size;
    Elf_Word sh_link;
    Elf_Word sh_info;
    Elf_Word sh_addralign;
    Elf_Word sh_entsize;
} Elf_Shdr;

typedef struct {
    Elf_Word p_type;
    Elf_Off  p_offset;
    Elf_Addr p_vaddr;
    Elf_Addr p_paddr;
    Elf_Word p_filesz;
    Elf_Word p_memsz;
    Elf_Word p_flags;
    Elf_Word p_align;
} Elf_Phdr;
