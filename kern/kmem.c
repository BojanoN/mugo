#include <arch/paging.h>
#include <kern/util.h>
#include <stdlib.h>
#include <sys/mem_cb.h>

#include <arch/info.h>
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

__attribute__((section(".stack"))) uint8_t kstack[CONF_KSTACK_SIZE];

vaddr_t kstack_top = (vaddr_t)kstack + CONF_KSTACK_SIZE;

static kernel_info_t* kinfo;

/*
 * Paging routines
 */

vaddr_t mmap(vaddr_t vaddr, paddr_t paddr, vaddr_t page_table_ptr, unsigned int flags)
{
    return 0;
}

static vaddr_t kmap(vaddr_t vaddr, paddr_t paddr, size_t size, unsigned int flags)
{
    ASSERT_MSG((vaddr >= (vaddr_t)&K_HIGH_VMA), "Attempted to page below K_HIGH_VMA");

    ASSERT(vaddr % ARCH_PAGE_SIZE == 0);
    ASSERT(paddr % ARCH_PAGE_SIZE == 0);
    ASSERT(size % ARCH_PAGE_SIZE == 0);

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

        if (flags & KMAP_PROT_USER) {
            pg_flags |= ARCH_PAGE_ENTRY_USER;
        }
    }

load:
    for (size_t offset = 0; offset < size; offset += ARCH_PAGE_SIZE) {

        size_t pt_offset             = ((vaddr + offset) & ARCH_VADDR_PT_OFFSET_MASK) >> 12;
        kernel_pt.entries[pt_offset] = ((paddr + offset) & PAGE_MASK) | pg_flags;
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

    log(ERROR, "Page fault at 0x%08x!", faulting_address);
    log(ERROR, "Page not present: %d\nRO: %d\nUser-mode: %d\nCause: %s", present, rw, us, id ? "Instruction fetch" : "Direct access");
    halt();
}

/*
 * Boot module loading routines/structs
 */

int bootstrap_mmap(vaddr_t vaddr, size_t size, unsigned int flags);

struct bootstrap_mman {
    proc_t*        active_proc;
    mem_callback_t callbacks;
} bootstrap_mman = {
    NULL,
    { bootstrap_mmap }
};

static paddr_t bootstrap_fetch_page(void)
{

    paddr_t page = kinfo->memmaps[0].addr;
    kinfo->memmaps[0].addr += ARCH_PAGE_SIZE;
    kinfo->memmaps[0].len -= ARCH_PAGE_SIZE;
    return page;
}

/* Identity mapping of necessary physical memory for initial server modules and paging structures */
void bootstrap_identity_map_init_mem(kernel_info_t* info)
{
    paddr_t phys_start = info->memmaps[0].addr;
    paddr_t phys_end   = phys_start;

    ASSERT(phys_start % ARCH_PAGE_SIZE == 0);

    paddr_t bootstrap_pt_phys   = (vaddr_t)&bootstrap_pt - info->kernel_high_vma;
    size_t  bootstrap_pd_offset = (phys_start & ARCH_VADDR_PD_OFFSET_MASK) >> 22;

    page_directory.entries[bootstrap_pd_offset] = ((arch_pd_entry_t)bootstrap_pt_phys & ARCH_PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);

    /* Determine amount of pages for bootstraping modules */
    for (size_t i = 0; i < info->no_modules; i++) {

        struct boot_module* module      = &info->modules[i];
        size_t              module_size = module->end_paddr - module->start_paddr;

        /* Each module needs a page directory along with at least two page tables, and another page for the stack */
        phys_end += ARCH_PAGE_SIZE * 4;

        phys_end += (module_size & ARCH_PAGE_MASK) + ARCH_PAGE_SIZE;
    }
    // TODO: Map
}

paddr_t bootstrap_create_page_dir(void)
{
    paddr_t page = bootstrap_fetch_page();

    bootstrap_mman.active_proc->page_dir = page;

    return page;
}

int bootstrap_mmap(vaddr_t vaddr, size_t size, unsigned int flags)
{
    /*
    ASSERT(bootstrap_mman->active_proc != NULL);

    paddr_t pd        = bootstrap_mman.active_proc->page_dir;
    size_t  pd_offset = (vaddr & ARCH_VADDR_PD_OFFSET_MASK) >> 22;
    size_t  pt_offset = (vaddr & ARCH_VADDR_PT_OFFSET_MASK) >> 12;

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

        if (flags & KMAP_PROT_USER) {
            pg_flags |= ARCH_PAGE_ENTRY_USER;
        }
    }

load:

    // kernel_pt.entries[pt_offset] = (paddr & PAGE_MASK) | pg_flags;
    */
    return 0;
}

/*
 * Kernel mapping routines
 */

extern unsigned int __kernel_heap_start;

void kmem_init(kernel_info_t* info)
{
    log(INFO, "Remapping kernel");
    kinfo = info;

    paddr_t pd_phys           = (paddr_t)((vaddr_t)&page_directory - (unsigned int)kinfo->kernel_high_vma);
    paddr_t bootstrap_pt_phys = ((paddr_t)&bootstrap_pt) - (paddr_t)kinfo->kernel_high_vma;

    size_t kernel_pd_offset                  = ((vaddr_t)&K_HIGH_VMA & ARCH_VADDR_PD_OFFSET_MASK) >> 22;
    page_directory.entries[kernel_pd_offset] = (((vaddr_t)&kernel_pt - (vaddr_t)&K_HIGH_VMA) & PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
    page_directory.entries[0]                = ((pd_entry_t)bootstrap_pt_phys & ARCH_PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);

    map_kernel_memory();

    arch_load_pagetable(pd_phys);

    log(INFO, "Initializing kheap");
    kmalloc_init((uint8_t*)kinfo->kheap_range.start, (uint8_t*)kinfo->kheap_phys_range.start, CONF_KHEAP_SIZE);
}

extern unsigned int __early_start, __kernel_start, __kernel_text_end, __kernel_bss_end, __kernel_rodata_end, __kernel_stack_guard_start, __kernel_stack_end, __kernel_end, __kernel_stack_start, __ktable_end;

void map_kernel_memory(void)
{
    vaddr_t kernel_start       = (vaddr_t)&__kernel_start;
    vaddr_t ktable_end         = (vaddr_t)&__ktable_end;
    vaddr_t kernel_text_end    = (vaddr_t)&__kernel_text_end;
    vaddr_t kernel_bss_end     = (vaddr_t)&__kernel_bss_end;
    vaddr_t kernel_rodata_end  = (vaddr_t)&__kernel_rodata_end;
    vaddr_t kernel_data_end    = (vaddr_t)&__kernel_stack_guard_start;
    vaddr_t kernel_stack_start = (vaddr_t)&__kernel_stack_start;
    vaddr_t kernel_stack_end   = (vaddr_t)&__kernel_stack_end;

    paddr_t kernel_start_phys = (paddr_t)kernel_start - (vaddr_t)&K_HIGH_VMA; //(paddr_t)&__early_start;
    paddr_t curr_addr_phys    = kernel_start_phys;
    vaddr_t curr_addr         = kernel_start;

    size_t size = ktable_end - kernel_start;

    // Identity map first MB for now
    // Clear bootostrap mappings
    memset(&bootstrap_pt.entries[0], 0, sizeof(page_table_t));
    size_t current_pt_offset = 0;

    for (unsigned int addr = 0; addr < 0x100000; addr += PAGE_SIZE, current_pt_offset++) {
        bootstrap_pt.entries[current_pt_offset] = (addr & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // .ktable
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

    // .heap
    ASSERT(kinfo->memmaps[0].len > CONF_KHEAP_SIZE);

    paddr_t heap_start = kinfo->memmaps[0].addr;
    kinfo->memmaps[0].addr += CONF_KHEAP_SIZE;
    kinfo->memmaps[0].len -= CONF_KHEAP_SIZE;

    curr_addr_phys = heap_start;

    kinfo->kheap_phys_range.start = heap_start;
    kinfo->kheap_range.start      = curr_addr;

    size = CONF_KHEAP_SIZE;
    kmap(curr_addr, curr_addr_phys, size, KMAP_PROT_READ | KMAP_PROT_WRITE);

    // Map VGA to  0xC03FF000
    kmap(0xC03FF000, 0x000B8000, ARCH_PAGE_SIZE, KMAP_PROT_READ | KMAP_PROT_WRITE);
}
