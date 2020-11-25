#include "mem/phys_alloc.h"
#include <arch/multiboot.h>
#include <arch/paging.h>
#include <kern/kprint.h>
#include <kern/util.h>
#include <types.h>

static frame_allocator_t kernel_frame_allocator = { 0 };

/*
 * Paging routines
 */
unsigned int kmap()
{
    return 0;
}

unsigned int mmap()
{
    return 0;
}

/*
 * Kernel mapping routines
 */

void map_kernel_memory(void)
{
    extern frame_allocator_t region_allocators[NO_PHYS_MEM_REGION];

    kernel_frame_allocator = region_allocators[PHYS_MEM_REGION_KERNEL];

    unsigned int allocated_frame = kernel_frame_allocator.alloc();

    kprintf("Allocated frame physical address: 0x%08x\n", allocated_frame);

    // TODO: identity map the first MB

    // TODO: adjust the ldscript so we can map individual sections with appropriate permissions
}

/*
 * Kernel malloc
 */
/*
void* kmalloc(size_t size)
{
    return 0;
}
*/
