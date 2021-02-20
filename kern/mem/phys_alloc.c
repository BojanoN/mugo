#include "phys_alloc.h"
#include "../kmem.h"

#include <arch/paging.h>
#include <kern/kprint.h>
#include <kern/util.h>

extern vaddr_t K_METADATA_VMA, K_METADATA_SIZE, K_HIGH_VMA, __kernel_start, __kernel_end;

typedef struct metadata_region_info {
    paddr_t start;
    paddr_t end;
    paddr_t current_offset;
} metadata_region_info_t;

phys_mem_region_t memory_regions[NO_PHYS_MEM_REGION]    = { 0 };
frame_allocator_t region_allocators[NO_PHYS_MEM_REGION] = { 0 };

static size_t                 total_frames_available = 0;
static metadata_region_info_t metadata_region_info   = {
    0,
    0,
    0
};

static const size_t DMA_REGION_MIN_SIZE    = 1 << 20; // 1 MB
static const size_t KERNEL_REGION_MIN_SIZE = 1 << 20; // 1 MB

void init_free_memory(struct multiboot_info* multiboot_info_ptr)
{
    paddr_t mmap_length   = multiboot_info_ptr->mmap_length;
    paddr_t mmap_addr     = multiboot_info_ptr->mmap_addr;
    paddr_t mmap_end_addr = mmap_addr + mmap_length;

    paddr_t kernel_size          = (paddr_t)&__kernel_end - (paddr_t)&__kernel_start;
    paddr_t metadata_region_size = (paddr_t)&K_METADATA_SIZE;

    metadata_region_info.start          = (paddr_t)&K_METADATA_VMA;
    metadata_region_info.current_offset = metadata_region_info.start;
    metadata_region_info.end            = metadata_region_info.start + metadata_region_size;

    kprintf("Detected %d MB of free memory\n", multiboot_info_ptr->mem_upper / 1024);

    if ((DMA_REGION_MIN_SIZE + KERNEL_REGION_MIN_SIZE) > (multiboot_info_ptr->mem_upper * 1024)) {
        panic("Insufficient physical memory available!");
    }

    // Scan the memory map and determine total number of available frames
    paddr_t                 curr_addr = mmap_addr;
    multiboot_memory_map_t* entry;

    while (curr_addr < mmap_end_addr) {
        entry = (multiboot_memory_map_t*)curr_addr;

        paddr_t region_addr_start = entry->addr;
        paddr_t region_addr_end   = entry->addr + entry->len;
        paddr_t region_no_frames  = entry->len / PAGE_SIZE;

        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE
            || (mmap_addr >= region_addr_start && mmap_addr <= region_addr_end)) {
            curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
            continue;
        }

        total_frames_available += region_no_frames;
        curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
    }

    total_frames_available -= (kernel_size / PAGE_SIZE) + (metadata_region_size / PAGE_SIZE);
    kprintf("%d frames available\n", total_frames_available);

#ifdef DEBUG
    kprintf("The kernel occupies %d frames\n", (kernel_size / PAGE_SIZE) + (metadata_region_size / PAGE_SIZE));
#endif

    // initialize kernel memory region
    paddr_t kernel_end_page_align      = ((paddr_t)&__kernel_end & 0xFFFFF000) + PAGE_SIZE;
    paddr_t kernel_memory_region_size  = (2 << 20); // 2 MB
    paddr_t kernel_memory_region_start = (kernel_end_page_align - (paddr_t)&K_HIGH_VMA);

    curr_addr                                    = mmap_addr;
    memory_regions[PHYS_MEM_REGION_KERNEL].size  = kernel_memory_region_size;
    memory_regions[PHYS_MEM_REGION_KERNEL].start = kernel_memory_region_start;
    memory_regions[PHYS_MEM_REGION_KERNEL].end   = kernel_memory_region_start + kernel_memory_region_size;

    // TODO: fill with stub allocator
    /*
    extern frame_allocator_t stack_allocator;
    region_allocators[PHYS_MEM_REGION_KERNEL] = stack_allocator;
    stack_allocator.init(multiboot_info_ptr, &memory_regions[PHYS_MEM_REGION_KERNEL]);
    */
}

paddr_t reserve_metadata_space(size_t size)
{
    if ((metadata_region_info.current_offset + size) > metadata_region_info.end) {
        return NULL;
    }

    paddr_t ret = metadata_region_info.current_offset;
    metadata_region_info.current_offset += size;

    return ret;
}
