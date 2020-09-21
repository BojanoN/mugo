#include <arch/info.h>

#include "boot/gdt.h"
#include "drivers/pic.h"
#include "drivers/timer.h"
#include "idt.h"
#include "util.h"

extern void kernel_init(arch_info_t*);

static arch_info_t info = { 0 };

void init(void)
{
    init_gdt();
    PIC_init();
    init_idt();
    init_timer(100000);
}

void arch_entrypoint(arch_info_t* i)
{
    info.name = "x86";
    info.init = init;

    kernel_init(&info);
}
