#include "util.h"

#include <arch/panic.h>
#include <console.h>
#include <kern/kprint.h>

NORETURN void panic(const char* msg)
{
    console_clear_screen();
    kprintf("!!!PANIC!!!\n%s\n", msg);
    arch_panic_hook();
    halt();

    __builtin_unreachable();
}
