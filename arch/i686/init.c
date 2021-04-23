#include <arch/interrupt.h>
#include <arch/objs.h>
#include <arch/util.h>

#include "boot/gdt.h"
#include "drivers/pic.h"
#include "drivers/timer.h"
#include "regs.h"

#include "idt.h"


void       page_fault_handler(irq_context_t registers);
extern struct tss tss;

void arch_kcall_entry(void);

void arch_init(void)
{
  int x = 1;
  x++;
  init_gdt();
    PIC_init();
     init_idt();

  register_interrupt_callback(0xe, page_fault_handler);

    // Configure sysenter
    msr_write(I386_SYSENTER_CS, 0x8);
    msr_write(I386_SYSENTER_EIP, (uint32_t)&arch_kcall_entry);
    msr_write(I386_SYSENTER_ESP, (uint32_t)&tss.sp0);
}
