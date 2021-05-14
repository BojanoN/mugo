#pragma once

#include "elf_base.h"

typedef struct {
    Elf_Hdr*  elf_hdr;
    Elf_Shdr* section_headers;
    Elf_Shdr* symbol_table;
    Elf_Shdr* string_table;

    Elf_Phdr* program_headers;

    size_t size;
} elf_ctx_t;

struct exec_info;

int elf_ctx_create(elf_ctx_t* ctx, uint8_t* buf, size_t size);
int elf_ctx_load(elf_ctx_t* ctx, struct exec_info* info);
