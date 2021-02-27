#include <arch/paging.h>
#include <kern/util.h>
#include <stdlib.h>

#include <types/base.h>

#include "assert.h"
#include "conf.h"
#include "kmalloc.h"
#include "kmem.h"

extern unsigned int K_HIGH_VMA;

__attribute__((section(".ktable"))) page_directory_t page_directory = { 0 };
__attribute__((section(".ktable"))) page_table_t     kernel_pt      = { 0 };

__attribute__((section(".stack"))) uint8_t stack[CONF_STACK_SIZE];

/*
 * Paging routines
 */

vaddr_t mmap()
{
    return 0;
}

vaddr_t kmap(vaddr_t vaddr, paddr_t paddr, unsigned int flags, unsigned int flush)
{
    ASSERT_MSG((vaddr >= (vaddr_t)&K_HIGH_VMA), "Attempted to page below K_HIGH_VMA");

    size_t pt_offset = (vaddr & ARCH_VADDR_PT_OFFSET_MASK) >> 12;

    ASSERT_MSG(pt_offset < 1024, "Kernel PT overflow");

    paddr_t pg_flags = 0;

    if (flags != KMAP_PROT_NONE) {
        unsigned int wr = flags & KMAP_PROT_WRITE;
        unsigned int rd = flags & KMAP_PROT_READ;

        if (!rd && !wr) {
            log(WARN, "No PROT_[READ | WRITE] specified; mapping 0x%08x as PROT_NONE", vaddr);
            goto load;
        }

        pg_flags = ARCH_PAGE_PRESENT;

        if (wr) {
            pg_flags |= ARCH_PAGE_RW;
        }
    }

load:
    kernel_pt.entries[pt_offset] = (paddr & PAGE_MASK) | pg_flags;

    if (flush) {
        arch_flush_tlb_single(vaddr);
    }

    return 0;
}

void page_fault_handler(irq_context_t registers)
{
    vaddr_t faulting_address = arch_get_faulting_addr();

    int present = !(registers.err_code & 0x1); // Page not present
    int rw      = registers.err_code & 0x2; // Write operation?
    int us      = registers.err_code & 0x4; // Processor was in user-mode?
    // int reserved = registers.err_code & 0x8; // Overwritten CPU-reserved bits of page entry?
    int id = registers.err_code & 0x10; // Caused by an instruction fetch?

    log(ERROR, "Page fault at 0x%08x!\n", faulting_address);
    log(ERROR, "Page not present: %d\nRO: %d\nUser-mode: %d\nCause: %s\n", present, rw, us, id ? "Instruction fetch" : "Direct access");
    halt();
}

/*
 * Kernel mapping routines
 */

extern unsigned int __kernel_heap_start;

void kmem_init(void)
{
    log(INFO, "Remapping kernel");
    memset(&page_directory, 0, sizeof(page_directory_t));

    size_t kernel_pd_offset                  = ((vaddr_t)&K_HIGH_VMA & ARCH_VADDR_PD_OFFSET_MASK) >> 22;
    page_directory.entries[kernel_pd_offset] = (((vaddr_t)&kernel_pt - (vaddr_t)&K_HIGH_VMA) & PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);

    map_kernel_memory();

    arch_load_pagetable((paddr_t)((vaddr_t)&page_directory - (vaddr_t)&K_HIGH_VMA));

    log(INFO, "Initializing kheap");
    kmalloc_init((uint8_t*)&__kernel_heap_start, (uint8_t*)((vaddr_t)&__kernel_heap_start - (vaddr_t)&K_HIGH_VMA), CONF_KHEAP_SIZE);
}

extern unsigned int __kernel_start, __kernel_text_end, __kernel_bss_end, __kernel_rodata_end, __kernel_stack_guard_start, __kernel_stack_end, __kernel_end;

void map_kernel_memory(void)
{
    vaddr_t kernel_start      = (vaddr_t)&__kernel_start;
    vaddr_t kernel_text_end   = (vaddr_t)&__kernel_text_end;
    vaddr_t kernel_bss_end    = (vaddr_t)&__kernel_bss_end;
    vaddr_t kernel_rodata_end = (vaddr_t)&__kernel_rodata_end;
    vaddr_t kernel_data_end   = (vaddr_t)&__kernel_stack_guard_start;
    vaddr_t kernel_stack_end  = (vaddr_t)&__kernel_stack_end;
    vaddr_t kernel_end        = (vaddr_t)&__kernel_stack_end;

    paddr_t kernel_start_phys = (paddr_t)kernel_start - (paddr_t)&K_HIGH_VMA;

    paddr_t curr_addr_phys = kernel_start_phys;
    vaddr_t curr_addr      = kernel_start;

    // .text
    for (; curr_addr < kernel_text_end; curr_addr += PAGE_SIZE, curr_addr_phys += PAGE_SIZE) {
        kmap(curr_addr, curr_addr_phys, KMAP_PROT_READ | KMAP_PROT_EXEC, 0);
    }

    // .bss
    for (; curr_addr < kernel_bss_end; curr_addr += PAGE_SIZE, curr_addr_phys += PAGE_SIZE) {
        kmap(curr_addr, curr_addr_phys, KMAP_PROT_WRITE | KMAP_PROT_READ, 0);
    }

    // .rodata
    for (; curr_addr < kernel_rodata_end; curr_addr += PAGE_SIZE, curr_addr_phys += PAGE_SIZE) {
        kmap(curr_addr, curr_addr_phys, KMAP_PROT_READ, 0);
    }

    // .data
    for (; curr_addr < kernel_data_end; curr_addr += PAGE_SIZE, curr_addr_phys += PAGE_SIZE) {
        kmap(curr_addr, curr_addr_phys, KMAP_PROT_READ | KMAP_PROT_WRITE, 0);
    }

    // stack guard page
    kmap(curr_addr, curr_addr_phys, KMAP_PROT_NONE, 0);
    curr_addr += PAGE_SIZE;
    curr_addr_phys += PAGE_SIZE;

    // .stack
    for (; curr_addr < kernel_stack_end; curr_addr += PAGE_SIZE, curr_addr_phys += PAGE_SIZE) {
        kmap(curr_addr, curr_addr_phys, KMAP_PROT_READ | KMAP_PROT_WRITE, 0);
    }

    // heap guard page
    kmap(curr_addr, 0, KMAP_PROT_NONE, 0);
    curr_addr += PAGE_SIZE;

    // heap
    for (; curr_addr < kernel_end; curr_addr += PAGE_SIZE, curr_addr_phys += PAGE_SIZE) {
        kmap(curr_addr, curr_addr_phys, KMAP_PROT_READ | KMAP_PROT_WRITE, 0);
    }
}
