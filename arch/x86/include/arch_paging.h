#pragma once
#include <types.h>

typedef uint32_t arch_pt_entry_t;
typedef uint32_t arch_pd_entry_t;

void arch_enable_paging(void);
void arch_load_pagetable(uint32_t pt_phys_addr);

#define ARCH_PAGE_SIZE 4096

#define ARCH_VADDR_PD_OFFSET_MASK   0xFFC00000
#define ARCH_VADDR_PT_OFFSET_MASK   0x003FF000
#define ARCH_VADDR_PAGE_OFFSET_MASK 0x00000FFF

#define ARCH_PD_ENTRY_PRESENT 0x1
#define ARCH_PD_ENTRY_RW      0x2

#define ARCH_PAGE_PRESENT ARCH_PD_ENTRY_PRESENT
#define ARCH_PAGE_RW      ARCH_PD_ENTRY_RW
