#include "boot/gdt.h"

extern void kernel_start(void);

void init(void)
{
    init_gdt();
    kernel_start();
}
