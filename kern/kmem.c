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

__attribute__((section(".ktable"))) page_directory_t boot_page_directory = { 0 };
__attribute__((section(".ktable"))) page_directory_t page_directory      = { 0 };
__attribute__((section(".ktable"))) page_table_t     kernel_pt           = { 0 };
__attribute__((section(".ktable"))) page_table_t     boot_kernel_pt      = { 0 };
__attribute__((section(".ktable"))) page_table_t     bootstrap_pt        = { 0 };
__attribute__((section(".ktable"))) page_table_t     module_bootstrap_pt = { 0 };

__attribute__((section(".stack"))) uint8_t kstack[CONF_KSTACK_SIZE];

vaddr_t kstack_top = (vaddr_t)kstack + CONF_KSTACK_SIZE;

static kernel_info_t* kinfo;

static exec_info_t k_exec_info = {
    (paddr_t)&page_directory,
    0,
    NULL
};

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
 * Boot module loading routines/structs
 */

static paddr_t bootstrap_fetch_page(void)
{
    paddr_t page = kinfo->memmaps[0].addr;
    ASSERT_MSG(kinfo->memmaps[0].len > ARCH_PAGE_SIZE, "Out of pages");

    kinfo->memmaps[0].addr += ARCH_PAGE_SIZE;
    kinfo->memmaps[0].len -= ARCH_PAGE_SIZE;
    return page;
}

/* Identity mapping of necessary physical memory for initial server modules and paging structures */
void bootstrap_identity_map_init_mem(kernel_info_t* info)
{
    paddr_t phys_start = info->memmaps[0].addr;
    paddr_t phys_end   = phys_start;
    kinfo              = info;

    ASSERT(phys_start % ARCH_PAGE_SIZE == 0);

    memcpy((void*)&boot_page_directory, (void*)&page_directory, sizeof(page_directory_t));

    /* Determine amount of pages for bootstraping modules */
    for (size_t i = 0; i < info->no_modules; i++) {

        struct boot_module* module      = &info->modules[i];
        size_t              module_size = module->end_paddr - module->start_paddr;

        /* Each module needs a page directory along with at least two page tables, and another page for the stack */
        phys_end += ARCH_PAGE_SIZE * 4;

        phys_end += (module_size & ARCH_PAGE_MASK) + ARCH_PAGE_SIZE;
    }

    // TODO: Map
    memset(&module_bootstrap_pt.entries[0], 0, sizeof(page_table_t));
    size_t current_pt_offset = 0;

    current_pt_offset = (phys_start & ARCH_VADDR_PT_OFFSET_MASK) >> ARCH_VADDR_PT_OFFSET;

    for (unsigned int addr = phys_start; addr < phys_end; addr += PAGE_SIZE, current_pt_offset++) {
        module_bootstrap_pt.entries[current_pt_offset] = (addr & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    info->bootstrap_memory_phys_range.start = phys_start;
    info->bootstrap_memory_phys_range.end   = phys_end;

    paddr_t pd_phys                  = (paddr_t)((vaddr_t)&boot_page_directory - (vaddr_t)info->kernel_high_vma);
    size_t  pd_offset                = arch_get_pt_offset(phys_start, ARCH_PD_1);
    paddr_t module_bootstrap_pt_phys = (vaddr_t)&module_bootstrap_pt - (paddr_t)info->kernel_high_vma;

    boot_page_directory.entries[pd_offset] = ((pd_entry_t)module_bootstrap_pt_phys & ARCH_PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);

    arch_load_pagedir(pd_phys);
}

paddr_t bootstrap_create_page_dir(void)
{
    return bootstrap_fetch_page();
}

int bootstrap_mmap_elf(exec_info_t* info, paddr_t paddr, vaddr_t vaddr, size_t size, unsigned int flags)
{
    unsigned int internal_flags = 0;

    unsigned int exec = flags & PF_X;
    unsigned int rd   = flags & PF_R;
    unsigned int wr   = flags & PF_W;

    internal_flags |= KMAP_PROT_EXEC * (exec != 0);
    internal_flags |= KMAP_PROT_READ * (rd != 0);
    internal_flags |= KMAP_PROT_WRITE * (wr != 0);

    internal_flags |= KMAP_PROT_USER;

    return bootstrap_mmap(info, paddr, vaddr, size, internal_flags);
}

int bootstrap_mmap(exec_info_t* info, paddr_t paddr, vaddr_t vaddr, size_t size, unsigned int flags)
{

    ASSERT(vaddr % ARCH_PAGE_SIZE == 0);
    ASSERT(paddr % ARCH_PAGE_SIZE == 0);
    ASSERT(size % ARCH_PAGE_SIZE == 0);

    paddr_t pg_flags = 0;

    unsigned int wr  = flags & KMAP_PROT_WRITE;
    unsigned int rd  = flags & KMAP_PROT_READ;
    unsigned int usr = flags & KMAP_PROT_USER;

    if (!rd && !wr) {
        log(WARN, "No PROT_[READ | WRITE] specified; mapping 0x%08x as PROT_NONE", vaddr);
    } else if (flags != KMAP_PROT_NONE) {
        pg_flags = ARCH_PAGE_PRESENT;
        pg_flags |= ARCH_PAGE_RW * (wr != 0);
        pg_flags |= ARCH_PAGE_ENTRY_USER * (usr != 0);
    }

    page_directory_t* pd = (page_directory_t*)info->page_directory;

    // Preallocate all necessary page tables
    page_table_t* current_pt = (page_table_t*)(pd->entries[arch_get_pt_offset(vaddr, ARCH_PD_1)] & ARCH_PAGE_MASK);

    size_t current_pt_overflow = ((arch_get_pt_offset(vaddr, ARCH_PD_0) + (size / ARCH_PAGE_SIZE)) > 1024) ? 1 : 0;
    size_t no_page_tables      = (size / ARCH_PT_ADDRESSABLE_BYTES) + current_pt_overflow + (current_pt == NULL ? 1 : 0);
    size_t pd_offset           = arch_get_pt_offset(vaddr, ARCH_PD_1);

    for (size_t i = 0; i < no_page_tables; i++) {
        if (pd->entries[pd_offset + i] == 0) {
            paddr_t pt = bootstrap_fetch_page();
            if (pt == 0) {
                panic("Out of free frames");
            }
            memset((void*)pt, 0, ARCH_PAGE_SIZE);

            pd->entries[pd_offset + i] = (pt & ARCH_PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW | (info != &k_exec_info ? ARCH_PD_ENTRY_USER : 0));
        }
    }

    current_pt = (page_table_t*)(pd->entries[pd_offset] & ARCH_PAGE_MASK);

    for (size_t offset = 0; offset < size; offset += ARCH_PAGE_SIZE) {
        size_t vaddr_pd_offset = arch_get_pt_offset(vaddr + offset, ARCH_PD_1);

        if (pd_offset != vaddr_pd_offset) {
            pd_offset  = vaddr_pd_offset;
            current_pt = (page_table_t*)(pd->entries[pd_offset] & ARCH_PAGE_MASK);
        }

        size_t  pt_offset              = arch_get_pt_offset(vaddr + offset, ARCH_PD_0);
        paddr_t frame_addr             = current_pt->entries[pt_offset] ? (current_pt->entries[pt_offset] & ARCH_PAGE_MASK) : (paddr == PAGE_ALLOCATE ? bootstrap_fetch_page() : (paddr + offset));
        current_pt->entries[pt_offset] = ((frame_addr)&PAGE_MASK) | pg_flags;
    }

    //arch_flush_tlb();

    return 0;
}

/*
 * Kernel mapping routines
 */

extern unsigned int __kernel_end;

static vaddr_t kmap(vaddr_t vaddr, paddr_t paddr, size_t size, unsigned int flags)
{
    return bootstrap_mmap(&k_exec_info, paddr, vaddr, size, flags);
}

void kmem_init(kernel_info_t* info)
{
    log(INFO, "Remapping kernel");
    kinfo = info;

    map_kernel_memory(kinfo);

    paddr_t pd_phys = (paddr_t)((vaddr_t)&page_directory - (unsigned int)kinfo->kernel_high_vma);

    arch_load_pagedir(pd_phys);

    // Map kheap

    log(INFO, "Initializing kheap");
    kmalloc_init((uint8_t*)kinfo->kheap_range.start, CONF_KHEAP_SIZE);
}

void kmem_init_cleanup(void)
{
    paddr_t pd_phys = (paddr_t)((vaddr_t)&boot_page_directory - (vaddr_t)kinfo->kernel_high_vma);
    arch_load_pagedir(pd_phys);
}

extern unsigned int __early_start, __kernel_start, __kernel_text_end, __kernel_bss_end, __kernel_rodata_end, __kernel_stack_guard_start, __kernel_stack_end, __kernel_stack_start, __ktable_end;

void map_kernel_memory(kernel_info_t* info)
{
    vaddr_t kernel_start       = (vaddr_t)&__kernel_start;
    vaddr_t ktable_end         = (vaddr_t)&__ktable_end;
    vaddr_t kernel_text_end    = (vaddr_t)&__kernel_text_end;
    vaddr_t kernel_bss_end     = (vaddr_t)&__kernel_bss_end;
    vaddr_t kernel_rodata_end  = (vaddr_t)&__kernel_rodata_end;
    vaddr_t kernel_data_end    = (vaddr_t)&__kernel_stack_guard_start;
    vaddr_t kernel_stack_start = (vaddr_t)&__kernel_stack_start;
    vaddr_t kernel_stack_end   = (vaddr_t)&__kernel_stack_end;

    paddr_t bootstrap_pt_phys = ((paddr_t)&bootstrap_pt) - (paddr_t)kinfo->kernel_high_vma;

    size_t kernel_pd_offset                  = ((vaddr_t)&K_HIGH_VMA & ARCH_VADDR_PD_OFFSET_MASK) >> ARCH_VADDR_PD_OFFSET;
    page_directory.entries[kernel_pd_offset] = (((vaddr_t)&kernel_pt - (vaddr_t)&K_HIGH_VMA) & PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
    page_directory.entries[0]                = ((pd_entry_t)bootstrap_pt_phys & ARCH_PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);

    // Identity map first MB for now
    // Clear bootstrap mappings
    memset(&bootstrap_pt.entries[0], 0, sizeof(page_table_t));
    size_t current_pt_offset = 0;

    for (unsigned int addr = 0; addr < 0x100000; addr += PAGE_SIZE, current_pt_offset++) {
        bootstrap_pt.entries[current_pt_offset] = (addr & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // Identity map loaded modules
    for (size_t i = 0; i < kinfo->no_modules; i++) {
        struct boot_module* module = &kinfo->modules[i];
        current_pt_offset          = (module->start_paddr & ARCH_VADDR_PT_OFFSET_MASK) >> ARCH_VADDR_PT_OFFSET;
        paddr_t end                = module->end_paddr;
        for (paddr_t addr = module->start_paddr; addr < end; addr += PAGE_SIZE, current_pt_offset++) {
            bootstrap_pt.entries[current_pt_offset] = (addr & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
        }
    }

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
    size = kernel_bss_end - kernel_text_end;
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
