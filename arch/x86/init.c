#include <arch/info.h>
#include <arch/interrupt.h>
#include <arch/multiboot.h>
#include <arch/objs.h>
#include <arch/util.h>

#include "boot/gdt.h"
#include "drivers/pic.h"
#include "drivers/timer.h"
#include "idt.h"
#include "regs.h"

extern void       page_fault_handler(irq_context_t registers);
extern struct tss tss;
void              syscall_entry(void);

void arch_init(void)
{
    disable_interrupts();

    init_gdt();
    PIC_init();
    init_idt();

    register_interrupt_callback(14, page_fault_handler);

    // Configure sysenter
    msr_write(I386_SYSENTER_EIP, (uint32_t)&syscall_entry);
    msr_write(I386_SYSENTER_ESP, (uint32_t)&tss.sp0);

    enable_interrupts();
}
