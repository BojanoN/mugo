#include <arch/info.h>
#include <arch/paging.h>

extern page_directory_t page_directory;
extern page_table_t     kernel_pt;
extern page_table_t     metadata_pt;

// ldscript symbols
extern unsigned int __kernel_start, __kernel_end, K_HIGH_VMA;
extern unsigned int __early_start, __early_end;
extern unsigned int __metadata_start, __metadata_end, K_METADATA_VMA;

__attribute__((section(".boot.data"))) uint8_t       early_stack[0x1000] = { 0 };
__attribute__((section(".boot.data"))) kernel_info_t kinfo               = { 0 };

__attribute__((section(".boot.text"))) void early_console_write_string(const char* str)
{
    static uint16_t x = 0;
    static uint16_t y = 0;

    uint16_t* term_buff = (uint16_t*)0x00B8000;

    char* ptr = (char*)str;

    for (; *ptr; ptr++) {

        if (*ptr == '\n') {
            y++;
            x = 0;
            continue;
        }
        if (*ptr == '\r') {
            x = 0;
            continue;
        }

        if (x >= 80) {
            x = 0;
            y++;
        }

        x++;

        uint16_t chr = (*ptr << 4) | ((0x0 << 4) | 0xF);

        term_buff[(y * 80) + x] = chr;
    }
}

__attribute__((section(".boot.text"))) void early_panic(const char* str)
{
    early_console_write_string(str);
    asm("hlt\t\n");
}
__attribute__((section(".boot.text"))) void
early_page_map()
{
    unsigned int  k_vma               = (unsigned int)&K_HIGH_VMA;
    page_table_t* kernel_pt_phys      = (page_table_t*)((unsigned int)&kernel_pt - k_vma);
    page_table_t* metadata_pt_phys    = (page_table_t*)((unsigned int)&metadata_pt - k_vma);
    page_table_t* page_directory_phys = (page_table_t*)((unsigned int)&page_directory - k_vma);

    unsigned int current_pt_offset = 0;

    unsigned int k_phys_start  = (unsigned int)((unsigned int)&__kernel_start - k_vma);
    unsigned int k_phys_end    = (unsigned int)((unsigned int)&__kernel_end - k_vma);
    unsigned int k_start_vaddr = (unsigned int)((unsigned int)&__kernel_start);

    unsigned int early_phys_start = (unsigned int)&__early_start;
    unsigned int early_phys_end   = (unsigned int)&__early_end;

    unsigned int metadata_phys_start  = ((unsigned int)&__metadata_start & 0xFFFFF000) + PAGE_SIZE;
    unsigned int metadata_phys_end    = (unsigned int)&__metadata_end;
    unsigned int metadata_vaddr_start = (unsigned int)&K_METADATA_VMA;

    // Zero tables
    for (unsigned int i = 0; i < (PAGE_SIZE / 4) * 3; i++) {
        *(unsigned int*)((unsigned int)&page_directory + i) = 0;
    }

    unsigned int phys_addr = 0;
    current_pt_offset      = 0;

    // Identity map the first MB
    for (unsigned int i = 0; i < 0x100000; i += PAGE_SIZE, current_pt_offset++) {
        phys_addr                                  = i;
        kernel_pt_phys->entries[current_pt_offset] = (phys_addr & 0xFFFFF000) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // PT offset for kernel mapping
    current_pt_offset             = (k_start_vaddr & ARCH_VADDR_PT_OFFSET_MASK) >> 12;
    unsigned int kernel_pd_offset = (k_start_vaddr & ARCH_VADDR_PD_OFFSET_MASK) >> 22;

    // Map the kernel
    for (unsigned int i = 0; i < ((k_phys_end - k_phys_start) + PAGE_SIZE); i += PAGE_SIZE, current_pt_offset++) {
        phys_addr                                  = k_phys_start + i;
        kernel_pt_phys->entries[current_pt_offset] = (phys_addr & 0xFFFFF000) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    current_pt_offset = (early_phys_start & ARCH_VADDR_PT_OFFSET_MASK) >> 12;

    for (unsigned int i = 0; i < ((early_phys_end - early_phys_start) + PAGE_SIZE); i += PAGE_SIZE, current_pt_offset++) {
        phys_addr                                  = early_phys_start + i;
        kernel_pt_phys->entries[current_pt_offset] = (phys_addr & 0xFFFFF000) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // Metadata region mapping

    current_pt_offset               = (metadata_vaddr_start & ARCH_VADDR_PT_OFFSET_MASK) >> 12;
    unsigned int metadata_pd_offset = (metadata_vaddr_start & ARCH_VADDR_PD_OFFSET_MASK) >> 22;

    for (unsigned int i = 0; i < ((metadata_phys_end - metadata_phys_start) + PAGE_SIZE); i += PAGE_SIZE, current_pt_offset++) {
        phys_addr                                    = metadata_phys_start + i;
        metadata_pt_phys->entries[current_pt_offset] = (phys_addr & 0xFFFFF000) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // Map VGA to  0xC03FF000
    kernel_pt_phys->entries[1023] = ((0x000B8000 & 0xFFFFF000) | 0x003);

    page_directory_phys->entries[0]                  = ((arch_pd_entry_t)kernel_pt_phys & 0xFFFFF000) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
    page_directory_phys->entries[kernel_pd_offset]   = ((arch_pd_entry_t)kernel_pt_phys & 0xFFFFF000) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
    page_directory_phys->entries[metadata_pd_offset] = ((arch_pd_entry_t)metadata_pt_phys & 0xFFFFF000) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
}

__attribute__((section(".boot.text"))) paddr_t early_main(uint32_t multiboot_magic, struct multiboot_info* multiboot_info_ptr)
{

    if (multiboot_magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        early_panic("The kernel was not booted by a multiboot compliant bootloader!");
    }

    if (!(multiboot_info_ptr->flags & MULTIBOOT_INFO_MEM_MAP)) {
        early_panic("No Multiboot memory map was found!");
    }

    // TODO: clear boot PT mappings
    if (!(multiboot_info_ptr->flags & MULTIBOOT_INFO_MEMORY)) {
        early_panic("Unable to probe for free memory!");
    }

    paddr_t mmap_length   = multiboot_info_ptr->mmap_length;
    paddr_t mmap_addr     = multiboot_info_ptr->mmap_addr;
    paddr_t mmap_end_addr = mmap_addr + mmap_length;

    // paddr_t kernel_size = (paddr_t)&__kernel_end - (paddr_t)&__kernel_start;

    // Scan the memory map and determine total number of available frames
    paddr_t                 curr_addr = mmap_addr;
    multiboot_memory_map_t* entry;
    int                     saved_mmap_index = 0;

    while (curr_addr < mmap_end_addr) {
        entry = (multiboot_memory_map_t*)curr_addr;

        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
            continue;
        }

        kinfo.memmaps[saved_mmap_index] = *entry;
        saved_mmap_index++;
        curr_addr += entry->size + sizeof(multiboot_memory_map_t*);
    }

    kinfo.mmap_size = saved_mmap_index + 1;

    early_page_map();

    return (paddr_t)&kinfo;
}
