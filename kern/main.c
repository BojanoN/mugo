#include <arch/info.h>
#include <arch/interrupt.h>
#include <arch/multiboot.h>
#include <arch/paging.h>
#include <arch/thread.h>

#include <kern/kmem.h>
#include <kern/kprint.h>
#include <kern/util.h>
#include <stdlib.h>

#include <console.h>
#include <stdlib.h>

#include "assert.h"
#include "kmalloc.h"
#include "log.h"
#include "sched.h"
#include "time.h"

extern sched_policy_t sched_rr;
static kernel_info_t  kinfo;
//static tcb_t          init_tcb;

void arch_init(void);
void arch_return_to_user(void);

void bootstrap(kernel_info_t* info)
{
    native_disable_interrupts();

    memcpy(&kinfo, info, sizeof(kernel_info_t));

    console_init();

    log(INFO, "Bootstrap started");
    log(INFO, "Initializing kernel memory");

    kmem_init();

    arch_init();

    //   memset(&init_tcb, 0, sizeof(tcb_t));

    //    init_tcb.id = 0;

    //native_create_thread(&init_tcb, (vaddr_t)test_func, (vaddr_t)test_stack, 0x1000, (vaddr_t)test_kstack, 0x1000);

    // sched_init(&sched_rr, &init_tcb);

    //ktime_init(sched_rr.interrupt_handler);

    //arch_return_to_user();
}
