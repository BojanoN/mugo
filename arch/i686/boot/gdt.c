#include "gdt.h"
#include "../include/arch_obj_defs.h"
#include "../include/arch_util.h"
#include <types/base.h>

//extern unsigned int K_HIGH_VMA;
//extern uint8_t* kstack;

gdt_entry_t gdt_entries[6];
gdt_t       gdt;
struct tss  tss = { 0 };

extern void load_gdt(uint32_t);
extern void load_tss(void);

static void fill_gdt_entry(int32_t segment_num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    gdt_entry_t* tmp = &gdt_entries[segment_num];

    tmp->base_low    = base & 0xFFFF;
    tmp->base_middle = (base >> 16) & 0xFF;
    tmp->base_high   = (base >> 24) & 0xFF;

    tmp->limit_low = limit & 0xFFFF;

    tmp->access      = access;
    tmp->granularity = (limit >> 16) & 0x0F;
    tmp->granularity |= granularity;
}

void init_gdt(void)
{
    gdt.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt.base  = (uint32_t)&gdt_entries;

    // Null entry
    fill_gdt_entry(0, 0, 0, 0, 0);
    // Kernel code entry
    fill_gdt_entry(1, 0x0, 0xFFFFFFFF, 0x9A, 0xC0);
    // Kernel data entry
    fill_gdt_entry(2, 0x0, 0xFFFFFFFF, 0x92, 0xC0);
    // Usermode entry
    fill_gdt_entry(3, 0x0, 0xFFFFFFFF, 0xFA, 0xC0);
    // usermode data entry
    fill_gdt_entry(4, 0x0, 0xFFFFFFFF, 0xF2, 0xC0);
    // TSS entry
    fill_gdt_entry(5, (uint32_t)&tss, sizeof(struct tss), 0x89, 0xC0);

    tss.ss0 = 0x10;
    tss.sp0 = 0; //(uint32_t)kstack + 0x4000;
    tss.cs  = 0x0b;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;

    load_gdt((uint32_t)&gdt);
    load_tss();
};

void native_update_tss(vaddr_t esp0)
{
    tss.sp0 = (uint32_t)esp0;
}
