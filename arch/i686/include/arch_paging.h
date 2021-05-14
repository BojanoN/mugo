#pragma once
#include <arch_util.h>
#include <types/base.h>

typedef paddr_t arch_pt_entry_t;
typedef paddr_t arch_pd_entry_t;

#define ARCH_PAGE_SIZE 4096

#define ARCH_PAGE_MASK               0xFFFFF000
#define ARCH_VADDR_PD_OFFSET_MASK    0xFFC00000
#define ARCH_VADDR_PT_OFFSET_MASK    0x003FF000
#define ARCH_PT_ENTRY_PHYS_ADDR_MASK 0x00000FFF
#define ARCH_VADDR_PD_OFFSET         22
#define ARCH_VADDR_PT_OFFSET         12

#define ARCH_PD_ENTRY_PRESENT 0x1
#define ARCH_PD_ENTRY_RW      0x2
#define ARCH_PD_ENTRY_USER    0x4

#define ARCH_PAGE_PRESENT             ARCH_PD_ENTRY_PRESENT
#define ARCH_PAGE_RW                  ARCH_PD_ENTRY_RW
#define ARCH_PAGE_ENTRY_USER          0x4
#define ARCH_PAGE_ENTRY_WRITE_THRU    0x8
#define ARCH_PAGE_ENTRY_CACHE_DISABLE 0x10
#define ARCH_PAGE_ENTRY_EXEC          0x0 // not avaialable for x86

#define ARCH_PD_0 0
#define ARCH_PD_1 1
#pragma clang poison ARCH_PD_2 ARCH_PD_3

static inline void arch_load_pagetable(paddr_t pd_phys_addr)
{
    native_cr3_write(pd_phys_addr);
}

static inline void arch_flush_tlb_single(vaddr_t addr)
{
    asm volatile("invlpg (%0)" ::"r"(addr)
                 : "memory");
}

static inline void arch_flush_tlb(void)
{
    native_cr3_write(native_cr3_read());
}

static inline vaddr_t arch_get_faulting_addr(void)
{
    return native_cr2_read();
}

static inline size_t arch_get_pt_offset(vaddr_t vaddr, size_t level)
{
    switch (level) {
    case ARCH_PD_0:
        return (size_t)((vaddr & ARCH_VADDR_PT_OFFSET_MASK) >> ARCH_VADDR_PT_OFFSET);
    case ARCH_PD_1:
        return (size_t)((vaddr & ARCH_VADDR_PD_OFFSET_MASK) >> ARCH_VADDR_PD_OFFSET);
    default:
        return 0;
    }
}
