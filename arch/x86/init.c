#include <arch/info.h>
#include <arch/util.h>

#include "boot/gdt.h"
#include "drivers/pic.h"
#include "drivers/timer.h"
#include "idt.h"

extern void kernel_init(arch_info_t*, unsigned long, unsigned long);
extern void init_paging(void);

static arch_info_t info = { 0 };

void init(void)
{
    init_gdt();
    PIC_init();
    init_idt();
    //init_timer(10000);
    //init_paging();
}

void arch_entrypoint(unsigned long magic, unsigned long multiboot_tag_addr)
{
    info.name = "x86";
    info.init = init;

    kernel_init(&info, magic, multiboot_tag_addr);
}
