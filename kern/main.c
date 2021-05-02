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
#include <sys/elf.h>

#include "assert.h"
#include "kmalloc.h"
#include "log.h"
#include "sched.h"
#include "time.h"

extern sched_policy_t sched_rr;
kernel_info_t         kinfo;
//static tcb_t          init_tcb;

void arch_init(void);
void arch_return_to_user(void);

void load_boot_modules(void)
{
    size_t    no_modules = kinfo.no_modules;
    elf_ctx_t module_ctx;
    int       err;

    // identity map the necessary memory range for easier initialization

    //    extern struct bootstrap_mman bootstrap_mman;

    memset(&module_ctx, 0, sizeof(elf_ctx_t));

    for (size_t i = 0; i < no_modules; i++) {

        struct boot_module* module       = &kinfo.modules[i];
        vaddr_t             module_start = module->start_paddr;
        size_t              module_size  = module->end_paddr - module->start_paddr;
        //        proc_t*             proc         = (proc_t*)kmalloc(sizeof(proc_t));

        log(INFO, "Loading module %s...", module->name);

        if ((err = elf_ctx_create(&module_ctx, (uint8_t*)module_start, module_size)) != ELF_OK) {
            log(ERR, "Failed to load module %s: %s", module->name, elf_strerror(err));
            panic("Unable to load boot modules!");
        }

        if ((err = elf_ctx_load(&module_ctx, NULL)) != ELF_OK) {
            log(ERR, "Failed to load module %s: %s", module->name, elf_strerror(err));
            panic("Unable to load boot modules!");
        }

        // TODO: assign one page stack
    }
}

void bootstrap(kernel_info_t* info)
{
    native_disable_interrupts();

    memcpy(&kinfo, info, sizeof(kernel_info_t));

    console_init();

    log(INFO, "Bootstrap started");
    bootstrap_identity_map_init_mem(&kinfo);

    load_boot_modules();

    log(INFO, "Initializing kernel memory");
    kmem_init(&kinfo);

    arch_init();

    //   memset(&init_tcb, 0, sizeof(tcb_t));

    //    init_tcb.id = 0;

    //native_create_thread(&init_tcb, (vaddr_t)test_func, (vaddr_t)test_stack, 0x1000, (vaddr_t)test_kstack, 0x1000);

    // sched_init(&sched_rr, &init_tcb);

    //ktime_init(sched_rr.interrupt_handler);

    //arch_return_to_user();
}
