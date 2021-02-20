#pragma once

#include <arch/multiboot.h>
#include <types/base.h>

#define NO_PHYS_MEM_REGION 3

#define PHYS_MEM_REGION_DMA    0
#define PHYS_MEM_REGION_NORMAL 1
#define PHYS_MEM_REGION_KERNEL 2

typedef struct phys_mem_region {
    paddr_t start;
    paddr_t end;

    paddr_t no_frames_available;
    paddr_t no_frames_in_use;

    size_t size;

} phys_mem_region_t;

typedef void (*frame_allocator_init)(struct multiboot_info*, phys_mem_region_t*);
typedef paddr_t (*alloc_frame)(void);
typedef void (*free_frame)(paddr_t);

typedef struct frame_allocator {
    frame_allocator_init init;
    alloc_frame          alloc;
    free_frame           free;
} frame_allocator_t;

paddr_t reserve_metadata_space(size_t size);
