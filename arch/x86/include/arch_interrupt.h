#pragma once

#include "../drivers/pic.h"
#include "arch_types_base.h"

#define NO_INTERRUPTS 256

#define enable_interrupts()  asm("sti \n")
#define disable_interrupts() asm("cli \n")

struct irq_context {

    uint32_t ds; // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t int_no, err_code; // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
};

#define arch_mask_irq(num)   PIC_mask_irq(num)
#define arch_unmask_irq(num) PIC_unmask_irq(num)

#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47
