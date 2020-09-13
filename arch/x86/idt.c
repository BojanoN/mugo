#include "idt.h"
#include "include/interrupt.h"

idt_entry_t idt_entries[256] = { 0 };
idt_t       idt;

extern void irh0(void);
extern void irh1(void);
extern void irh2(void);
extern void irh3(void);
extern void irh4(void);
extern void irh5(void);
extern void irh6(void);
extern void irh7(void);
extern void irh8(void);
extern void irh9(void);
extern void irh10(void);
extern void irh11(void);
extern void irh12(void);
extern void irh13(void);
extern void irh14(void);
extern void irh15(void);
extern void irh16(void);
extern void irh17(void);
extern void irh18(void);
extern void irh19(void);
extern void irh20(void);
extern void irh21(void);
extern void irh22(void);
extern void irh23(void);
extern void irh24(void);
extern void irh25(void);
extern void irh26(void);
extern void irh27(void);
extern void irh28(void);
extern void irh29(void);
extern void irh30(void);
extern void irh31(void);

extern void load_idt(uint32_t);

static void fill_idt_entry(uint32_t num, uint32_t offset, uint16_t selector, uint8_t type)
{
    idt_entry_t* tmp = &idt_entries[num];

    tmp->offset_low  = offset & 0xFFFF;
    tmp->offset_high = (offset >> 16) & 0xFFFF;

    tmp->selector = selector;
    tmp->zero     = 0;

    tmp->type_attr = type /* | 0x60 */;
}

void init_idt(void)
{
    idt.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt.base  = (uint32_t)&idt_entries;

    fill_idt_entry(0, (uint32_t)irh0, 0x08, 0x8E);
    fill_idt_entry(1, (uint32_t)irh1, 0x08, 0x8E);
    fill_idt_entry(2, (uint32_t)irh2, 0x08, 0x8E);
    fill_idt_entry(3, (uint32_t)irh3, 0x08, 0x8E);
    fill_idt_entry(4, (uint32_t)irh4, 0x08, 0x8E);
    fill_idt_entry(5, (uint32_t)irh5, 0x08, 0x8E);
    fill_idt_entry(6, (uint32_t)irh6, 0x08, 0x8E);
    fill_idt_entry(7, (uint32_t)irh7, 0x08, 0x8E);
    fill_idt_entry(8, (uint32_t)irh8, 0x08, 0x8E);
    fill_idt_entry(9, (uint32_t)irh9, 0x08, 0x8E);
    fill_idt_entry(10, (uint32_t)irh10, 0x08, 0x8E);
    fill_idt_entry(11, (uint32_t)irh11, 0x08, 0x8E);
    fill_idt_entry(12, (uint32_t)irh12, 0x08, 0x8E);
    fill_idt_entry(13, (uint32_t)irh13, 0x08, 0x8E);
    fill_idt_entry(14, (uint32_t)irh14, 0x08, 0x8E);
    fill_idt_entry(15, (uint32_t)irh15, 0x08, 0x8E);
    fill_idt_entry(16, (uint32_t)irh16, 0x08, 0x8E);
    fill_idt_entry(17, (uint32_t)irh17, 0x08, 0x8E);
    fill_idt_entry(18, (uint32_t)irh18, 0x08, 0x8E);
    fill_idt_entry(19, (uint32_t)irh19, 0x08, 0x8E);
    fill_idt_entry(20, (uint32_t)irh20, 0x08, 0x8E);
    fill_idt_entry(21, (uint32_t)irh21, 0x08, 0x8E);
    fill_idt_entry(22, (uint32_t)irh22, 0x08, 0x8E);
    fill_idt_entry(23, (uint32_t)irh23, 0x08, 0x8E);
    fill_idt_entry(24, (uint32_t)irh24, 0x08, 0x8E);
    fill_idt_entry(25, (uint32_t)irh25, 0x08, 0x8E);
    fill_idt_entry(26, (uint32_t)irh26, 0x08, 0x8E);
    fill_idt_entry(27, (uint32_t)irh27, 0x08, 0x8E);
    fill_idt_entry(28, (uint32_t)irh28, 0x08, 0x8E);
    fill_idt_entry(29, (uint32_t)irh29, 0x08, 0x8E);
    fill_idt_entry(30, (uint32_t)irh30, 0x08, 0x8E);
    fill_idt_entry(31, (uint32_t)irh31, 0x08, 0x8E);

    load_idt((uint32_t)&idt);
}
