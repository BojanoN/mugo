#include <arch/interrupt.h>
#include <arch/objs.h>
#include <arch/util.h>
#include <kern/kmem.h>

#include "boot/gdt.h"
#include "drivers/pic.h"
#include "drivers/timer.h"
#include "regs.h"

#include "idt.h"

extern struct tss tss;

void arch_sysenter_stub(void);

void arch_init(void)
{
    init_gdt();
    PIC_init();
    init_idt();

    register_interrupt_callback(0xe, page_fault_handler);

    // Configure sysenter
    msr_write(I386_SYSENTER_CS, 0x8);
    msr_write(I386_SYSENTER_EIP, (uint32_t)&arch_sysenter_stub);
    msr_write(I386_SYSENTER_ESP, (uint32_t)tss.sp0);
}
