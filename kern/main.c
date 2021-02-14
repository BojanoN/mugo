#include <arch/info.h>
#include <arch/interrupt.h>
#include <arch/multiboot.h>
#include <arch/paging.h>
#include <arch/time.h>

#include <kern/kmem.h>
#include <kern/kprint.h>
#include <kern/util.h>

#include <console.h>
#include <string.h>

unsigned char stack[0x10000];

page_table_t page_table;

const char* msg = "Hi!\n";

static arch_info_t arch_info;

void kernel_main(void)
{
    kprintf("%s %c %d\n", msg, '?', 42);

    int x = *(int*)0xA000000;
    x += 1;
    asm volatile("int $0x3");
}

extern timer_t arch_timer;

void kernel_init(arch_info_t* info, uint32_t multiboot_magic, struct multiboot_info* multiboot_info_ptr)
{
    console_init();

    if (multiboot_magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("The kernel was not booted by a multiboot compliant bootloader!");
    }

    if (!(multiboot_info_ptr->flags & MULTIBOOT_INFO_MEM_MAP)) {
        panic("No Multiboot memory map was found!");
    }

    // TODO: clear boot PT mappings
    if (!(multiboot_info_ptr->flags & MULTIBOOT_INFO_MEMORY)) {
        panic("Unable to probe for free memory!");
    }

    if (!(multiboot_info_ptr->flags & MULTIBOOT_INFO_ELF_SHDR)) {
        panic("No ELF section headers provided!");
    }

    init_free_memory(multiboot_info_ptr);

    map_kernel_memory();

    arch_info = *info;
    arch_info.init();

    arch_timer.timer_init(10);

    kernel_main();
}
