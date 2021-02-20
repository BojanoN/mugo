#pragma once

#include <arch/multiboot.h>
#include <types/base.h>

#define NO_MEMMAPS 20

typedef struct {
    /* Straight multiboot-provided info */
    multiboot_info_t       mb_info;
    multiboot_memory_map_t memmaps[NO_MEMMAPS];
    paddr_t                mem_high_phys;
    size_t                 mmap_size;

    vaddr_t vir_kern_start; /* kernel addrspace starts */
    vaddr_t bootstrap_start, bootstrap_len;
    int     kernel_allocated_bytes; /* used by kernel */
    int     kernel_allocated_bytes_dynamic; /* used by kernel (runtime) */
} kernel_info_t;
