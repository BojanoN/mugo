#include "gdt.h"
#include "../include/arch_obj_defs.h"

extern unsigned int K_HIGH_VMA;

gdt_entry_t gdt_entries[5];
gdt_t       gdt;
struct tss  tss = { 0 };

extern void load_gdt(uint32_t);

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
    gdt.limit = (sizeof(gdt_entry_t) * 5);
    gdt.base  = (uint32_t)&gdt_entries;

    // Null entry
    fill_gdt_entry(0, 0, 0, 0, 0);
    // Kernel code entry
    fill_gdt_entry(1, 0x0, 0xFFFFFFFF, 0x9A, 0xC0);
    // Kernel data entry
    fill_gdt_entry(2, 0x0, 0xFFFFFFFF, 0x92, 0xC0);
    // Usermode entry
    fill_gdt_entry(3, 0x0, 0xC0000000, 0xFA, 0xC0);
    // TSS entry
    fill_gdt_entry(4, (uint32_t)&tss, sizeof(struct tss), 0x89, 0xC0);

    load_gdt((uint32_t)&gdt);
};
