#include <arch/paging.h>
#include <exec.h>
#include <kern/util.h>
#include <stdlib.h>

#include <arch/info.h>
#include <sys/elf_base.h>
#include <types/base.h>

#include "assert.h"
#include "conf.h"
#include "kmalloc.h"
#include "kmem.h"
#include "kobjs.h"

extern unsigned int K_HIGH_VMA;
extern exec_info_t  k_exec_info;

__attribute__((section(".stack")))
uint8_t kstack[CONF_KSTACK_SIZE];
vaddr_t kstack_top = (vaddr_t)kstack + CONF_KSTACK_SIZE;

/*
 * Paging routines
 */

vaddr_t
mmap(vaddr_t vaddr, paddr_t paddr, vaddr_t page_table_ptr, unsigned int flags)
{
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

    log(ERROR, "Page fault at 0x%08x!", faulting_address);
    log(ERROR, "Page not present: %d\nRO: %d\nUser-mode: %d\nCause: %s", present, rw, us, id ? "Instruction fetch" : "Direct access");
    halt();
}

/*
 * Kernel mapping routines
 */

static kernel_info_t* kinfo;

void kmem_init(kernel_info_t* info)
{

    kinfo = info;
    bootstrap_init_kinfo(info);

    log(INFO, "Remapping kernel");
    kmem_map_kernel_memory(kinfo);
    bootstrap_load_kernel_mappings();

    log(INFO, "Initializing kheap");
    kmalloc_init((uint8_t*)kinfo->kheap_range.start, CONF_KHEAP_SIZE);
}

void kmem_init_cleanup(void)
{
    bootstrap_load_boot_mappings();
}

extern unsigned int __early_start, __kernel_start, __kernel_end, __kernel_text_end, __kernel_bss_end, __kernel_bss_start, __kernel_rodata_end, __kernel_stack_guard_start, __kernel_stack_end, __kernel_stack_start, __ktable_end;

void kmem_map_kernel_memory(kernel_info_t* info)
{
    vaddr_t kernel_start       = (vaddr_t)&__kernel_start;
    vaddr_t ktable_end         = (vaddr_t)&__ktable_end;
    vaddr_t kernel_text_end    = (vaddr_t)&__kernel_text_end;
    vaddr_t kernel_bss_start   = (vaddr_t)&__kernel_bss_start;
    vaddr_t kernel_bss_end     = (vaddr_t)&__kernel_bss_end;
    vaddr_t kernel_rodata_end  = (vaddr_t)&__kernel_rodata_end;
    vaddr_t kernel_data_end    = (vaddr_t)&__kernel_stack_guard_start;
    vaddr_t kernel_stack_start = (vaddr_t)&__kernel_stack_start;
    vaddr_t kernel_stack_end   = (vaddr_t)&__kernel_stack_end;

    bootstrap_map_boot_modules();

    paddr_t kernel_start_phys = (paddr_t)kernel_start - (vaddr_t)&K_HIGH_VMA; //(paddr_t)&__early_start;
    paddr_t curr_addr_phys    = kernel_start_phys;
    vaddr_t curr_addr         = kernel_start;

    // .ktable
    size_t size = ktable_end - kernel_start;
    kmap(curr_addr, curr_addr_phys, size, KMAP_PROT_READ | KMAP_PROT_WRITE);
    curr_addr_phys += size;
    curr_addr += size;

    // .text
    size = kernel_text_end - ktable_end;
    kmap(curr_addr, curr_addr_phys, size, KMAP_PROT_READ | KMAP_PROT_EXEC);
    curr_addr_phys += size;
    curr_addr += size;

    // .bss
    size = kernel_bss_end - kernel_bss_start;
    kmap(curr_addr, curr_addr_phys, size, KMAP_PROT_WRITE | KMAP_PROT_READ);
    curr_addr_phys += size;
    curr_addr += size;

    // .rodata
    size = kernel_rodata_end - kernel_bss_end;
    kmap(curr_addr, curr_addr_phys, size, KMAP_PROT_READ);
    curr_addr_phys += size;
    curr_addr += size;

    // .data
    size = kernel_data_end - kernel_rodata_end;
    kmap(curr_addr, curr_addr_phys, size, KMAP_PROT_READ | KMAP_PROT_WRITE);
    curr_addr_phys += size;
    curr_addr += size;

    // stack guard page

    kmap(curr_addr, 0, ARCH_PAGE_SIZE, KMAP_PROT_NONE);
    curr_addr += PAGE_SIZE;
    curr_addr_phys += PAGE_SIZE;

    // .stack
    size = kernel_stack_end - kernel_stack_start;
    kmap(curr_addr, curr_addr_phys, size, KMAP_PROT_READ | KMAP_PROT_WRITE);
    curr_addr_phys += size;
    curr_addr += size;

    extern unsigned int __console_debug;
    kmap((vaddr_t)&__console_debug, 0x000B8000, ARCH_PAGE_SIZE, KMAP_PROT_READ | KMAP_PROT_WRITE);
    curr_addr += ARCH_PAGE_SIZE;

    ASSERT(info->memmaps[0].len > CONF_KHEAP_SIZE);
    vaddr_t kheap_start = curr_addr;
    kmap(kheap_start, PAGE_ALLOCATE, CONF_KHEAP_SIZE, KMAP_PROT_READ | KMAP_PROT_WRITE);

    info->kheap_range.start = kheap_start;
}
