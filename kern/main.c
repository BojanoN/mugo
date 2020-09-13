#include <kern/kprint.h>
#include <string.h>

unsigned char stack[0x10000];

const char* msg = "Hi!\n";

void kernel_start(void)
{
    console_init();

    kprintf("%s %c %d\n", msg, '?', 42);
    asm volatile("int $0x3");
}
