#pragma once

#include <arch/multiboot.h>
#include <types.h>

#define NO_PHYS_MEM_REGION 3

#define PHYS_MEM_REGION_DMA    0
#define PHYS_MEM_REGION_NORMAL 1
#define PHYS_MEM_REGION_KERNEL 2

typedef struct phys_mem_region {
    unsigned int start;
    unsigned int end;

    unsigned int no_frames_available;
    unsigned int no_frames_in_use;

    size_t size;

} phys_mem_region_t;

typedef void (*frame_allocator_init)(struct multiboot_info*, phys_mem_region_t*);
typedef unsigned int (*alloc_frame)(void);
typedef void (*free_frame)(unsigned int);

typedef struct frame_allocator {
    frame_allocator_init init;
    alloc_frame          alloc;
    free_frame           free;
} frame_allocator_t;

unsigned int reserve_metadata_space(size_t size);
