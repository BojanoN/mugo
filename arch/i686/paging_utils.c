#include "./include/arch_paging.h"

void bootstrap_on_load(void)
{
    // TODO: map sysenter stub page
}

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
        phys_end += ARCH_PAGE_SIZE * 8;

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
