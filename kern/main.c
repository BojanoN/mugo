#include <arch/info.h>
#include <arch/multiboot2.h>
#include <arch/paging.h>
#include <arch/util.h>
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

void kernel_start(void)
{
    kprintf("%s %c %d\n", msg, '?', 42);
    asm volatile("int $0x3");
}

void kernel_init(arch_info_t* info, unsigned long magic, unsigned long multiboot_tag_addr)
{

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        panic("Multiboot2 header magic invalid!");
    }

    arch_info = info;
    arch_info->init();

    console_init();
    kernel_start();
}
