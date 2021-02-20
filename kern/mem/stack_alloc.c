#include "phys_alloc.h"

#include <arch/paging.h>
#include <kern/kprint.h>
#include <kern/util.h>

static void    stack_alloc_init(struct multiboot_info* multiboot_info_ptr, phys_mem_region_t* region);
static paddr_t stack_alloc_frame(void);
static void    stack_free_frame(paddr_t addr);

static int      free_frames_stack_top = -1;
static paddr_t* free_frames_stack     = NULL;

frame_allocator_t stack_allocator = {
    stack_alloc_init,
    stack_alloc_frame,
    stack_free_frame,
};

static phys_mem_region_t* memory_region = NULL;

void stack_alloc_init(struct multiboot_info* multiboot_info_ptr, phys_mem_region_t* region)
{
    size_t  mmap_length   = multiboot_info_ptr->mmap_length;
    paddr_t mmap_addr     = multiboot_info_ptr->mmap_addr;
    paddr_t mmap_end_addr = mmap_addr + mmap_length;

    paddr_t no_frames_required = ((region->size / PAGE_SIZE) * 4) / PAGE_SIZE;

    if (no_frames_required == 0) {
        no_frames_required = 1;
    }

    memory_region               = region;
    region->no_frames_in_use    = 0;
    region->no_frames_available = 0;

    free_frames_stack = (paddr_t*)reserve_metadata_space((no_frames_required)*4);

    if (free_frames_stack == NULL) {
        panic("No space left in metadata region!");
    }

    // Scan the memory map and push all page addresses on the stack
    paddr_t curr_addr    = mmap_addr;
    paddr_t region_start = region->start;

    size_t                  no_frames_acquired = 0;
    multiboot_memory_map_t* entry;

    while (curr_addr < mmap_end_addr && (no_frames_acquired < no_frames_required)) {
        entry = (multiboot_memory_map_t*)curr_addr;

        paddr_t mmap_region_addr_start = entry->addr;
        paddr_t mmap_region_addr_end   = entry->addr + entry->len;

        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE
            || (mmap_addr >= mmap_region_addr_start && mmap_addr <= mmap_region_addr_end)) {
            curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
            continue;
        }

        if (!(region_start >= mmap_region_addr_start || region_start < mmap_region_addr_end)) {
            curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
            continue;
        }

        for (paddr_t addr = mmap_region_addr_start > region_start ? mmap_region_addr_start : region_start; (addr < mmap_region_addr_end) && (no_frames_acquired < no_frames_required); addr += PAGE_SIZE) {
            free_frames_stack_top++;
            no_frames_acquired++;

            free_frames_stack[free_frames_stack_top] = addr;
        }

        curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
    }

    if (no_frames_acquired != no_frames_required) {
        // this should probably never happen
        panic("Stack allocator: Insufficient memory!");
    }

    region->no_frames_available = no_frames_acquired;
}

static paddr_t stack_alloc_frame(void)
{
    paddr_t frame_addr;

    if (free_frames_stack_top >= 0) {
        frame_addr = free_frames_stack[free_frames_stack_top];
        free_frames_stack_top--;

        memory_region->no_frames_in_use++;
        memory_region->no_frames_available--;

        return frame_addr;
    }

    kprintf("Stack allocator: No free frames left!");
    return 0;
}

static void stack_free_frame(paddr_t addr)
{
    free_frames_stack_top++;
    free_frames_stack[free_frames_stack_top] = addr;

    memory_region->no_frames_in_use--;
    memory_region->no_frames_available++;
}
