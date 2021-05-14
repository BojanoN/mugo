//TODO: #include <log.h>
#include <exec.h>
#include <stdlib.h>
#include <sys/elf.h>

static uint8_t elf_magic[4] = {
    ELF_MAGIC_0,
    ELF_MAGIC_1,
    ELF_MAGIC_2,
    ELF_MAGIC_3,
};

static inline Elf_Shdr* elf_get_sheader(Elf_Hdr* hdr)
{
    return (Elf_Shdr*)((int)hdr + hdr->e_shoff);
}

static int elf_check_hdr(Elf_Hdr* hdr)
{
    for (size_t i = 0; i < sizeof(elf_magic); i++) {
        if (hdr->e_ident[i] != elf_magic[i]) {
            return ELF_INVALID_HDR;
        }
    }

    return ELF_OK;
}

static int elf_check_arch_supported(Elf_Hdr* hdr)
{
    if (hdr->e_ident[EIDX_CLASS] != ARCH_ELF_CLASS) {
        return ELF_UNSUPP_CLASS;
    }

    if (hdr->e_ident[EIDX_BYTE_ORDER] != ARCH_ELF_BYTE_ORDER) {
        return ELF_UNSUPP_BYTE_ORDER;
    }

    if (hdr->e_machine != ARCH_ELF_MACHINE) {
        return ELF_UNSUPP_MACHINE;
    }

    if (hdr->e_ident[EIDX_VERSION] != ARCH_ELF_TARGET_VERSION) {
        return ELF_UNSUPP_VERSION;
    }

    if (hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
        return ELF_INVALID_TYPE;
    }

    return ELF_OK;
}

int elf_ctx_create(elf_ctx_t* ctx, uint8_t* buf, size_t size)
{
    Elf_Hdr* hdr = (Elf_Hdr*)buf;
    int      err = 0;

    if ((err = elf_check_hdr(hdr)) != ELF_OK) {
        return err;
    }

    if ((err = elf_check_arch_supported(hdr)) != ELF_OK) {
        return err;
    }

    ctx->elf_hdr         = hdr;
    ctx->section_headers = elf_get_sheader(hdr);
    ctx->program_headers = (Elf_Phdr*)(buf + hdr->e_phoff);

    for (int i = 1; i < ctx->elf_hdr->e_shnum; i++) {
        Elf_Shdr* shdr = &ctx->section_headers[i];

        if (shdr->sh_type == SHT_SYMTAB) {
            ctx->symbol_table = shdr;
            ctx->string_table = &ctx->section_headers[shdr->sh_link];
        }
    }

    return ELF_OK;
}

int elf_ctx_load(elf_ctx_t* ctx, exec_info_t* info)
{
    if (!(info && info->mmap)) {
        return ELF_INVALID_HDR;
    }

    size_t no_pheaders = ctx->elf_hdr->e_phnum;

    for (size_t i = 0; i < no_pheaders; i++) {
        Elf_Phdr* phdr = &ctx->program_headers[i];

        if (phdr->p_type != PT_LOAD) {
            continue;
        }

        if ((phdr->p_filesz > phdr->p_memsz)) {
            return ELF_INVALID_PH;
        }

        if ((phdr->p_align < 2)
            && (phdr->p_align & (phdr->p_align - 1)) == 0
            && ((phdr->p_vaddr % phdr->p_align) != 0 || (phdr->p_offset % phdr->p_align) != 0)) {
            return ELF_INVALID_PH;
        }

        if (phdr->p_filesz == 0) {
            continue;
        }

        // TODO: flags
        // invoke mmap callback function, allocate physical frames and map header
        info->mmap(info, PAGE_ALLOCATE, phdr->p_vaddr, phdr->p_memsz, 0);
        // Copy header contents
        memcpy((void*)phdr->p_vaddr, (uint8_t*)ctx->elf_hdr + phdr->p_offset, phdr->p_filesz);
    }

    return ELF_OK;
}

uint8_t* elf_ctx_get_section(elf_ctx_t* ctx, Elf_Shdr* sec_hdr)
{
    return (uint8_t*)ctx->elf_hdr + sec_hdr->sh_offset;
}

#define X(f)  \
    case (f): \
        return #f;

const char* elf_strerror(enum ElfError err)
{
    switch (err) {
        ELF_ERR_ENUM

    default:
        return "UNKNOWN";
    }
}

#undef X
