#pragma once

#include <arch/info.h>
#include <arch_paging.h>
#include <exec.h>

typedef arch_pt_entry_t pt_entry_t;
typedef arch_pd_entry_t pd_entry_t;

typedef struct page_table {
    pt_entry_t entries[1024];
} page_table_t;

typedef struct page_directory {
    pd_entry_t entries[1024];
} page_directory_t;

#define PAGE_SIZE ARCH_PAGE_SIZE
#define PAGE_MASK ARCH_PAGE_MASK

void    bootstrap_init_kinfo(kernel_info_t* info);
void    bootstrap_load_boot_mappings(void);
void    bootstrap_load_kernel_mappings(void);
void    bootstrap_identity_map_init_mem(kernel_info_t* info);
void    bootstrap_map_boot_modules(void);
int     bootstrap_elf_mmap(exec_info_t* info, paddr_t paddr, vaddr_t vaddr, size_t size, unsigned int flags);
int     bootstrap_mmap(exec_info_t* info, paddr_t paddr, vaddr_t vaddr, size_t size, unsigned int flags);
void    bootstrap_elf_on_load(exec_info_t* info);
vaddr_t kmap(vaddr_t vaddr, paddr_t paddr, size_t size, unsigned int flags);
