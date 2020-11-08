#include <arch/paging.h>
#include <types.h>

extern page_directory_t page_directory;
extern page_table_t     kernel_pt;
extern page_table_t     boot_pt;

// ldscript symbols
extern unsigned int __kernel_start, __kernel_end, K_HIGH_VMA;
extern unsigned int __early_start, __early_end;

__attribute__((section(".boot.data"))) uint8_t early_stack[0x1000] = { 0 };

// TODO: add layer of abstraction for tty devices to easily switch between virtual and phys VGA
__attribute__((section(".boot.text"))) void early_page_map()
{
    unsigned int  k_vma               = (unsigned int)&K_HIGH_VMA;
    page_table_t* kernel_pt_phys      = (page_table_t*)((unsigned int)&kernel_pt - k_vma);
    page_table_t* boot_pt_phys        = (page_table_t*)((unsigned int)&boot_pt - k_vma);
    page_table_t* page_directory_phys = (page_table_t*)((unsigned int)&page_directory - k_vma);

    unsigned int current_pt_offset = 0;

    unsigned int k_phys_start  = (unsigned int)((unsigned int)&__kernel_start - k_vma);
    unsigned int k_phys_end    = (unsigned int)((unsigned int)&__kernel_end - k_vma);
    unsigned int k_start_vaddr = (unsigned int)((unsigned int)&__kernel_start);
    //    unsigned int k_end_vaddr   = (unsigned int)(&__kernel_end);

    unsigned int early_phys_start = (unsigned int)&__early_start;
    unsigned int early_phys_end   = (unsigned int)&__early_end;

    // Zero tables
    for (unsigned int i = 0; i < (PAGE_SIZE / 4) * 3; i++) {
        *(unsigned int*)((unsigned int)&page_directory + i) = 0;
    }

    // PT offset for kernel mapping
    current_pt_offset             = (k_start_vaddr & ARCH_VADDR_PT_OFFSET_MASK) >> 12;
    unsigned int kernel_pd_offset = (k_start_vaddr & ARCH_VADDR_PD_OFFSET_MASK) >> 22;
    unsigned int phys_addr;

    // Map the kernel
    for (unsigned int i = 0; i < ((k_phys_end - k_phys_start) + PAGE_SIZE); i += PAGE_SIZE, current_pt_offset++) {
        phys_addr = k_phys_start + i;

        kernel_pt_phys->entries[current_pt_offset] = (phys_addr & 0xFFFFF000) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    current_pt_offset = (early_phys_start & ARCH_VADDR_PT_OFFSET_MASK) >> 12;
    // unsigned int early_pd_offset = (early_start_vaddr & ARCH_VADDR_PD_OFFSET_MASK) >> 22;

    for (unsigned int i = 0; i < ((early_phys_end - early_phys_start) + PAGE_SIZE); i += PAGE_SIZE, current_pt_offset++) {
        phys_addr = early_phys_start + i;

        boot_pt_phys->entries[current_pt_offset] = (phys_addr & 0xFFFFF000) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // Map VGA to  0xC03FF000
    kernel_pt_phys->entries[1023] = ((0x000B8000 & 0xFFFFF000) | 0x003);

    page_directory_phys->entries[0]                = ((arch_pd_entry_t)boot_pt_phys & 0xFFFFF000) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
    page_directory_phys->entries[kernel_pd_offset] = ((arch_pd_entry_t)kernel_pt_phys & 0xFFFFF000) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
}

__attribute__((section(".boot.text"))) void early_main()
{
    early_page_map();
    return;
}
