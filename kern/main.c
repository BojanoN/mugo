#include <arch/info.h>
#include <arch/interrupt.h>
#include <arch/multiboot.h>
#include <arch/paging.h>

#include <kern/kmem.h>
#include <kern/kprint.h>
#include <kern/util.h>
#include <stdlib.h>

#include <console.h>
#include <string.h>

#include "assert.h"
#include "log.h"
#include "time.h"

const char* msg = "Hi!\n";

static kernel_info_t kinfo;

void arch_init(void);

void kernel_main(void)
{
    kprintf("%s %c %d\n", msg, '?', 42);

    asm volatile("int $0x3");
}

void bootstrap(kernel_info_t* info)
{
    memcpy(&kinfo, info, sizeof(kernel_info_t));

    console_init();

    log(INFO, "Bootstrap started");
    log(INFO, "Initializing kernel memory");

    kmem_init();

    arch_init();
    //ktime_init();

    kernel_main();
}
