#include <arch/info.h>
#include <arch/paging.h>
#include <kern/conf.h>

extern page_directory_t boot_page_directory;
extern page_table_t     boot_pt;
extern page_table_t     boot_kernel_pt;

// ldscript symbols
extern unsigned int __kernel_start, __ktable_start, __ktable_end, __kernel_end, K_HIGH_VMA;
extern unsigned int __early_start, __early_end;

__attribute__((section(".boot.data"))) uint8_t              early_stack[0x1000] = { 0 };
static __attribute__((section(".boot.data"))) kernel_info_t kinfo               = { 0 };

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
    page_table_t* kernel_pt_phys      = (page_table_t*)((unsigned int)&boot_kernel_pt - k_vma);
    page_table_t* page_directory_phys = (page_table_t*)((unsigned int)&boot_page_directory - k_vma);

    unsigned int current_pt_offset = 0;

    unsigned int k_phys_start  = (unsigned int)((unsigned int)&__ktable_start - k_vma);
    unsigned int k_phys_end    = (unsigned int)((unsigned int)&__kernel_end - k_vma);
    unsigned int k_start_vaddr = (unsigned int)((unsigned int)&__ktable_start);

    unsigned int early_phys_start = (unsigned int)&__early_start;
    unsigned int early_phys_end   = (unsigned int)&__early_end;

    paddr_t ktable_start_phys = ((paddr_t)&__ktable_start) - (paddr_t)k_vma;
    paddr_t ktable_end_phys   = ((paddr_t)&__ktable_end) - (paddr_t)k_vma;

    // Zero tables
    for (unsigned int i = 0; i < (PAGE_SIZE / 4); i++) {
        *(unsigned int*)((unsigned int)&boot_pt + i) = 0;
    }

    unsigned int phys_addr = 0;
    current_pt_offset      = 0;

    // Identity map the first MB
    for (unsigned int i = 0; i < 0x100000; i += PAGE_SIZE, current_pt_offset++) {
        phys_addr                          = i;
        boot_pt.entries[current_pt_offset] = (phys_addr & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // boot code mapping
    current_pt_offset = (early_phys_start & ARCH_VADDR_PT_OFFSET_MASK) >> 12;

    for (unsigned int i = 0; i < ((early_phys_end - early_phys_start) + PAGE_SIZE); i += PAGE_SIZE, current_pt_offset++) {
        phys_addr                          = early_phys_start + i;
        boot_pt.entries[current_pt_offset] = (phys_addr & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // PT offset for kernel mapping
    current_pt_offset             = (k_start_vaddr & ARCH_VADDR_PT_OFFSET_MASK) >> 12;
    unsigned int kernel_pd_offset = (k_start_vaddr & ARCH_VADDR_PD_OFFSET_MASK) >> 22;

    // Map the kernel
    for (unsigned int i = 0; i < ((k_phys_end - k_phys_start)); i += PAGE_SIZE, current_pt_offset++) {
        phys_addr                                  = k_phys_start + i;
        kernel_pt_phys->entries[current_pt_offset] = (phys_addr & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // Identity map frames following kernel frames - these will be used for paging structures
    phys_addr                          = kinfo.boot_module_phys_range.end;
    paddr_t paging_structs_mapping_end = phys_addr + ((CONF_KHEAP_SIZE / ARCH_PT_ADDRESSABLE_BYTES) + 2) * PAGE_SIZE;
    current_pt_offset                  = (phys_addr & ARCH_VADDR_PT_OFFSET_MASK) >> ARCH_VADDR_PT_OFFSET;

    for (unsigned int i = phys_addr; i < paging_structs_mapping_end; i += PAGE_SIZE, current_pt_offset++) {
        phys_addr                          = i;
        boot_pt.entries[current_pt_offset] = (phys_addr & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // identity map kernel page tables
    current_pt_offset = (ktable_start_phys & ARCH_VADDR_PT_OFFSET_MASK) >> ARCH_VADDR_PT_OFFSET;

    for (unsigned int i = ktable_start_phys; i < ktable_end_phys; i += PAGE_SIZE, current_pt_offset++) {
        phys_addr                          = i;
        boot_pt.entries[current_pt_offset] = (phys_addr & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW);
    }

    // Map VGA to
    extern vaddr_t __console_debug;
    current_pt_offset                          = ((vaddr_t)&__console_debug & ARCH_VADDR_PT_OFFSET_MASK) >> 12;
    kernel_pt_phys->entries[current_pt_offset] = ((0x00B8000 & ARCH_PAGE_MASK) | (ARCH_PAGE_PRESENT | ARCH_PAGE_RW));

    page_directory_phys->entries[0]                = ((arch_pd_entry_t)&boot_pt & ARCH_PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
    page_directory_phys->entries[kernel_pd_offset] = ((arch_pd_entry_t)kernel_pt_phys & ARCH_PAGE_MASK) | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
}

__attribute__((section(".boot.text"), optnone)) paddr_t early_main(uint32_t multiboot_magic, paddr_t multiboot_img_ptr)
{

    if (multiboot_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        early_panic("The kernel was not booted by a multiboot compliant bootloader!");
    }

    unsigned int k_vma = (unsigned int)&K_HIGH_VMA;

    kinfo.kernel_phys_range.start = (paddr_t)((unsigned int)&__kernel_start - k_vma);
    kinfo.kernel_phys_range.end   = (paddr_t)((unsigned int)&__kernel_end - k_vma);

    kinfo.bootstrap_memory_phys_range.start = kinfo.bootstrap_memory_phys_range.end = kinfo.kernel_phys_range.end;

    kinfo.kernel_high_vma   = (vaddr_t)k_vma;
    kinfo.kernel_size_bytes = kinfo.kernel_phys_range.end - kinfo.kernel_phys_range.start;

    // Adjust free physical pages in first mmap
    multiboot_memory_map_t* mmap = &kinfo.memmaps[0];
    mmap->len -= kinfo.kernel_size_bytes;
    mmap->addr = kinfo.kernel_phys_range.end;

    /* Parse multiboot tags */
    int saved_mmap_index = 0;
    int module_index     = 0;

    /* Module copying metadata */
    paddr_t modules_start        = kinfo.kernel_phys_range.end;
    paddr_t last_module_end_addr = kinfo.kernel_phys_range.end;
    size_t  total_module_size    = 0;

    kinfo.no_mmaps  = 0;
    kinfo.mem_upper = 0;

    for (struct multiboot_tag* tag = (struct multiboot_tag*)(multiboot_img_ptr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((multiboot_uint8_t*)tag + ((tag->size + 7) & ~7))) {

        switch (tag->type) {

        case MULTIBOOT_TAG_TYPE_MMAP: {
            multiboot_memory_map_t* entry         = 0;
            paddr_t                 mmap_end_addr = (paddr_t)(multiboot_uint8_t*)tag + tag->size;
            size_t                  entry_size    = ((struct multiboot_tag_mmap*)tag)->entry_size;

            for (paddr_t curr_addr = (paddr_t)((struct multiboot_tag_mmap*)tag)->entries;
                 curr_addr < mmap_end_addr && (saved_mmap_index + 1) < NO_MEMMAPS;
                 curr_addr += entry_size) {

                entry = (multiboot_memory_map_t*)curr_addr;

                if (entry->type != MULTIBOOT_MEMORY_AVAILABLE || entry->addr < 0x100000) {
                    continue;
                }

                multiboot_memory_map_t* dst = &kinfo.memmaps[saved_mmap_index];
                dst->addr                   = entry->addr;
                dst->len                    = entry->len;
                dst->type                   = entry->type;
                dst->zero                   = entry->zero;

                saved_mmap_index++;
                kinfo.no_mmaps++;
            }
            break;
        }
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            kinfo.mem_upper = ((struct multiboot_tag_basic_meminfo*)tag)->mem_upper;
            break;

        case MULTIBOOT_TAG_TYPE_MODULE: {
            if ((module_index + 1) > NO_MODULES) {
                break;
            }

            struct multiboot_tag_module* mb_module = (struct multiboot_tag_module*)tag;
            struct boot_module*          module    = &kinfo.modules[module_index];

            module->start_paddr = last_module_end_addr;
            // TODO: paranoia - check if the end_paddr is page aligned
            module->end_paddr = last_module_end_addr + (mb_module->mod_end - mb_module->mod_start);
            // page align
            module->end_paddr = (module->end_paddr & ~ARCH_PAGE_MASK) ? ((module->end_paddr & ARCH_PAGE_MASK) + ARCH_PAGE_SIZE) : module->end_paddr;

            /* Copy the module right next to the kernel */
            uint8_t* end = (uint8_t*)mb_module->mod_end;
            uint8_t* dst = (uint8_t*)module->start_paddr;

            total_module_size += (size_t)(module->end_paddr - module->start_paddr);

            for (uint8_t* src = (uint8_t*)mb_module->mod_start; src < end; src++, dst++) {
                *dst = *src;
            }

            last_module_end_addr = (paddr_t)module->end_paddr;

            char* p = mb_module->cmdline;
            int   i = 0;

            while (*p && (i < (MAX_MODULE_NAME_LEN - 1))) {
                module->name[i++] = *p++;
            }
            *p++ = 0;

            module_index++;
            kinfo.no_modules++;
            break;
        }
        default:
            break;
        }
    }

    if (!kinfo.no_mmaps) {
        early_panic("No Multiboot memory map was found!");
    }

    if (!kinfo.mem_upper) {
        early_panic("Unable to probe for free memory!");
    }

    if (!kinfo.no_modules) {
        early_panic("No modules loaded!");
    }

    kinfo.boot_module_phys_range.start = modules_start;
    kinfo.boot_module_phys_range.end   = last_module_end_addr;

    // TODO: check if sufficient memory is available

    early_page_map();

    paddr_t k_phys_end = (paddr_t)((vaddr_t)&__kernel_end - k_vma);

    kinfo.memmaps[0].len -= (size_t)(k_phys_end - kinfo.memmaps[0].addr);
    kinfo.memmaps[0].len -= total_module_size;
    kinfo.memmaps[0].addr = last_module_end_addr;

    return (paddr_t)&kinfo;
}
