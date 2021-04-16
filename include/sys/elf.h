#pragma once

#include <arch_elf.h>

#include "elf_ctx.h"

#define ELF_ERR_ENUM         \
    X(ELF_OK)                \
    X(ELF_UNSUPP_MACHINE)    \
    X(ELF_UNSUPP_CLASS)      \
    X(ELF_UNSUPP_BYTE_ORDER) \
    X(ELF_UNSUPP_VERSION)    \
    X(ELF_UNSUPP_PH_TYPE)    \
    X(ELF_INVALID_PH)        \
    X(ELF_INVALID_TYPE)      \
    X(ELF_INVALID_HDR)

#define X(f) f,

enum ElfError {
    ELF_ERR_ENUM
};

#undef X

const char* elf_strerror(enum ElfError err);
