#include "boot/gdt.h"
#include "drivers/pic.h"
#include "idt.h"

extern void kernel_start(void);

void init(void)
{
    init_gdt();
    PIC_init();
    init_idt();
    kernel_start();
}
