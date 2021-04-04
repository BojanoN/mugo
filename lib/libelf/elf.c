#include <arch_elf.h>
#include <sys/elf_base.h>

static uint8_t elf_magic[4] = {
    ELF_MAGIC_0,
    ELF_MAGIC_1,
    ELF_MAGIC_2,
    ELF_MAGIC_3,
};

int elf_check_hdr(Elf32_Hdr* hdr)
{
    for (size_t i = 0; i < sizeof(elf_magic); i++) {
        if (hdr->e_ident[i] != elf_magic[0]) {
            return -1;
        }
    }

    return 0;
}

int elf_check_arch_supported(Elf32_Hdr* hdr)
{
    if (hdr->e_ident[EIDX_CLASS] != ARCH_ELF_CLASS) {
        return -1;
    }

    if (hdr->e_ident[EIDX_BYTE_ORDER] != ARCH_ELF_BYTE_ORDER) {
        return -1;
    }

    if (hdr->e_machine != ARCH_ELF_MACHINE) {
        return -1;
    }

    if (hdr->e_ident[EIDX_VERSION] != ARCH_ELF_TARGET_VERSION) {
        return -1;
    }

    if (hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
        return -1;
    }

    return 0;
}

void elf_load_file(uint8_t* file)
{
    Elf32_Hdr* hdr = (Elf32_Hdr*)file;

    if (!elf_check_hdr(hdr)) {
        return;
    }

    if (!elf_check_arch_supported(hdr)) {
        return;
    }

    switch (hdr->e_type) {
    case ET_EXEC:
        break;
    case ET_REL:
        break;
    }

    return;
}
