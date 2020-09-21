#include <arch/info.h>
#include <kern/kprint.h>
#include <string.h>

unsigned char stack[0x10000];

const char* msg = "Hi!\n";

static arch_info_t* arch_info;

void kernel_start(void)
{

    kprintf("%s %c %d\n", msg, '?', 42);
    asm volatile("int $0x3");
}

void kernel_init(arch_info_t* info)
{
    arch_info = info;

    arch_info->init();

    console_init();
    kernel_start();
}
