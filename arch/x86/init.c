#include "boot/gdt.h"
#include "idt.h"

extern void kernel_start(void);

void init(void)
{
    init_gdt();
    init_idt();
    kernel_start();
}
