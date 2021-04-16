#pragma once

#include <arch/multiboot.h>
#include <types/base.h>
#include <types/mem.h>

#define NO_MEMMAPS 20
#define NO_MODULES 5

#define MAX_MODULE_NAME_LEN 32

struct boot_module {
    paddr_t start_paddr;
    paddr_t end_paddr;

    vaddr_t start_vaddr;

    char name[MAX_MODULE_NAME_LEN];
};

typedef struct {
    /* Straight multiboot-provided info */
    multiboot_memory_map_t memmaps[NO_MEMMAPS];
    size_t                 no_mmaps;

    size_t mem_upper;

    struct boot_module modules[NO_MODULES];
    size_t             no_modules;

    phys_mem_range_t kernel_phys_range;
    phys_mem_range_t kheap_phys_range;
    phys_mem_range_t bootstrap_memory_phys_range;

    mem_range_t kheap_range;

    vaddr_t kernel_high_vma;

    size_t kernel_size_bytes; /* used by kernel */
    size_t kernel_allocated_bytes_dynamic; /* used by kernel (runtime) */
} kernel_info_t;
