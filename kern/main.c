#include <arch/info.h>
#include <arch/multiboot.h>
#include <arch/paging.h>
#include <arch/util.h>

#include <kern/kmem.h>
#include <kern/kprint.h>

#include <string.h>

unsigned char stack[0x10000];

page_table_t page_table;

const char* msg = "Hi!\n";

static arch_info_t* arch_info;

void panic(const char* msg)
{
    console_clear_screen();
    kprintf("!!!PANIC!!!\n %s", msg);
    halt();
}

void kernel_main(void)
{
    kprintf("%s %c %d\n", msg, '?', 42);
    unsigned int addr = alloc_frame();
    kprintf("Allocated frame addr 0x%x\n", addr);

    asm volatile("int $0x3");
}

void kernel_init(arch_info_t* info, uint32_t multiboot_magic, struct multiboot_info* multiboot_info_ptr)
{

    console_init();

    if (multiboot_magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("The kernel was not booted by a multiboot compliant bootloader!");
    }

    if (!(multiboot_info_ptr->flags & (1 << 6))) {
        panic("No Multiboot memory map was found!\n");
    }

    // TODO: clear boot PT mappings
    if (!(multiboot_info_ptr->flags & 0x1)) {
        panic("Unable to probe for free memory!");
    }

    pageframe_alloc_init(multiboot_info_ptr);

    arch_info = info;
    arch_info->init();

    kernel_main();
}
