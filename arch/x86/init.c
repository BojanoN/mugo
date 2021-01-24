#include <arch/info.h>
#include <arch/multiboot.h>
#include <arch/util.h>

#include "boot/gdt.h"
#include "drivers/pic.h"
#include "drivers/timer.h"
#include "idt.h"

extern void kernel_init(arch_info_t*, uint32_t, struct multiboot_info*);
extern void init_paging(void);

static arch_info_t info = { 0 };

void init(void)
{
    init_gdt();
    PIC_init();
    init_idt();
}

void arch_entrypoint(uint32_t magic, struct multiboot_info* multiboot_info_ptr)
{
    info.name = "x86";
    info.init = init;

    kernel_init(&info, magic, multiboot_info_ptr);
}
