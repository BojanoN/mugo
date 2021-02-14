#include "mem/phys_alloc.h"
#include <arch/interrupt.h>
#include <arch/multiboot.h>
#include <arch/paging.h>
#include <arch/util.h>
#include <kern/kprint.h>
#include <kern/util.h>

#include <types.h>

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

void page_fault_handler(irq_context_t registers)
{

    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0"
                 : "=r"(faulting_address));

    // The error code gives us details of what happened.
    int present = !(registers.err_code & 0x1); // Page not present
    int rw      = registers.err_code & 0x2; // Write operation?
    int us      = registers.err_code & 0x4; // Processor was in user-mode?
    // int reserved = registers.err_code & 0x8; // Overwritten CPU-reserved bits of page entry?
    int id = registers.err_code & 0x10; // Caused by an instruction fetch?

    kprintf("Page fault at 0x%08x!\n", faulting_address);
    kprintf("Page not present: %d\nRO: %d\nUser-mode: %d\nCause: %s\n", present, rw, us, id ? "Instruction fetch" : "Direct access");
    halt();
}

/*
 * Kernel mapping routines
 */

extern phys_mem_region_t memory_regions[NO_PHYS_MEM_REGION];

void map_kernel_memory(void)
{

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
