#include "idt.h"
#include <arch/interrupt.h>

idt_entry_t idt_entries[256] = { 0 };
idt_t       idt;

extern void eh0(void);
extern void eh1(void);
extern void eh2(void);
extern void eh3(void);
extern void eh4(void);
extern void eh5(void);
extern void eh6(void);
extern void eh7(void);
extern void eh8(void);
extern void eh9(void);
extern void eh10(void);
extern void eh11(void);
extern void eh12(void);
extern void eh13(void);
extern void eh14(void);
extern void eh15(void);
extern void eh16(void);
extern void eh17(void);
extern void eh18(void);
extern void eh19(void);
extern void eh20(void);
extern void eh21(void);
extern void eh22(void);
extern void eh23(void);
extern void eh24(void);
extern void eh25(void);
extern void eh26(void);
extern void eh27(void);
extern void eh28(void);
extern void eh29(void);
extern void eh30(void);
extern void eh31(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

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

    // CPU exception handlers
    fill_idt_entry(0, (uint32_t)eh0, 0x08, 0x8E);
    fill_idt_entry(1, (uint32_t)eh1, 0x08, 0x8E);
    fill_idt_entry(2, (uint32_t)eh2, 0x08, 0x8E);
    fill_idt_entry(3, (uint32_t)eh3, 0x08, 0x8E);
    fill_idt_entry(4, (uint32_t)eh4, 0x08, 0x8E);
    fill_idt_entry(5, (uint32_t)eh5, 0x08, 0x8E);
    fill_idt_entry(6, (uint32_t)eh6, 0x08, 0x8E);
    fill_idt_entry(7, (uint32_t)eh7, 0x08, 0x8E);
    fill_idt_entry(8, (uint32_t)eh8, 0x08, 0x8E);
    fill_idt_entry(9, (uint32_t)eh9, 0x08, 0x8E);
    fill_idt_entry(10, (uint32_t)eh10, 0x08, 0x8E);
    fill_idt_entry(11, (uint32_t)eh11, 0x08, 0x8E);
    fill_idt_entry(12, (uint32_t)eh12, 0x08, 0x8E);
    fill_idt_entry(13, (uint32_t)eh13, 0x08, 0x8E);
    fill_idt_entry(14, (uint32_t)eh14, 0x08, 0x8E);
    fill_idt_entry(15, (uint32_t)eh15, 0x08, 0x8E);
    fill_idt_entry(16, (uint32_t)eh16, 0x08, 0x8E);
    fill_idt_entry(17, (uint32_t)eh17, 0x08, 0x8E);
    fill_idt_entry(18, (uint32_t)eh18, 0x08, 0x8E);
    fill_idt_entry(19, (uint32_t)eh19, 0x08, 0x8E);
    fill_idt_entry(20, (uint32_t)eh20, 0x08, 0x8E);
    fill_idt_entry(21, (uint32_t)eh21, 0x08, 0x8E);
    fill_idt_entry(22, (uint32_t)eh22, 0x08, 0x8E);
    fill_idt_entry(23, (uint32_t)eh23, 0x08, 0x8E);
    fill_idt_entry(24, (uint32_t)eh24, 0x08, 0x8E);
    fill_idt_entry(25, (uint32_t)eh25, 0x08, 0x8E);
    fill_idt_entry(26, (uint32_t)eh26, 0x08, 0x8E);
    fill_idt_entry(27, (uint32_t)eh27, 0x08, 0x8E);
    fill_idt_entry(28, (uint32_t)eh28, 0x08, 0x8E);
    fill_idt_entry(29, (uint32_t)eh29, 0x08, 0x8E);
    fill_idt_entry(30, (uint32_t)eh30, 0x08, 0x8E);
    fill_idt_entry(31, (uint32_t)eh31, 0x08, 0x8E);

    // IRQ handlers
    fill_idt_entry(32, (uint32_t)irq0, 0x08, 0x8E);
    fill_idt_entry(33, (uint32_t)irq1, 0x08, 0x8E);
    fill_idt_entry(34, (uint32_t)irq2, 0x08, 0x8E);
    fill_idt_entry(35, (uint32_t)irq3, 0x08, 0x8E);
    fill_idt_entry(36, (uint32_t)irq4, 0x08, 0x8E);
    fill_idt_entry(37, (uint32_t)irq5, 0x08, 0x8E);
    fill_idt_entry(38, (uint32_t)irq6, 0x08, 0x8E);
    fill_idt_entry(39, (uint32_t)irq7, 0x08, 0x8E);
    fill_idt_entry(40, (uint32_t)irq8, 0x08, 0x8E);
    fill_idt_entry(41, (uint32_t)irq9, 0x08, 0x8E);
    fill_idt_entry(42, (uint32_t)irq10, 0x08, 0x8E);
    fill_idt_entry(43, (uint32_t)irq11, 0x08, 0x8E);
    fill_idt_entry(44, (uint32_t)irq12, 0x08, 0x8E);
    fill_idt_entry(45, (uint32_t)irq13, 0x08, 0x8E);
    fill_idt_entry(46, (uint32_t)irq14, 0x08, 0x8E);
    fill_idt_entry(47, (uint32_t)irq15, 0x08, 0x8E);

    load_idt((uint32_t)&idt);
}
