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
#include <exec.h>
#include <stdlib.h>
#include <sys/elf.h>

#include "assert.h"
#include "kmalloc.h"
#include "log.h"
#include "sched.h"
#include "time.h"

static kernel_info_t kinfo;

void arch_init(void);
void arch_return_to_user(void);

static proc_t boot_processes[CONF_MAX_BOOT_MODULES]         = { 0 };
static tcb_t  boot_processes_threads[CONF_MAX_BOOT_MODULES] = { 0 };

void load_boot_modules(void)
{
    size_t                  no_modules = kinfo.no_modules;
    elf_ctx_t               module_ctx;
    exec_info_t             info = { 0 };
    int                     err;
    pid_t                   pid = 1;
    extern page_directory_t boot_page_directory;

    // identity map the necessary memory range for easier initialization

    ASSERT_MSG(no_modules < CONF_MAX_BOOT_MODULES, "Too much boot modules loaded");

    info.mmap = bootstrap_mmap_elf;

    for (size_t i = 0; i < no_modules; i++) {

        struct boot_module* module       = &kinfo.modules[i];
        vaddr_t             module_start = module->start_paddr;
        size_t              module_size  = module->end_paddr - module->start_paddr;
        proc_t*             proc         = &boot_processes[i];
        tcb_t*              thread       = &boot_processes_threads[i];

        paddr_t pd_addr = bootstrap_create_page_dir();
        if (pd_addr == 0) {
            panic("Unable to create page directory for module");
        }

        memset(&module_ctx, 0, sizeof(elf_ctx_t));

        memset((void*)pd_addr, 0, ARCH_PAGE_SIZE);
        info.page_directory = pd_addr;

        // Create paging structures, map kernel
        page_directory_t* module_pd = (page_directory_t*)pd_addr;
        memcpy((void*)module_pd, (void*)&boot_page_directory, sizeof(page_directory_t));

        arch_load_pagedir(pd_addr);

        log(INFO, "Loading module %s...", module->name);

        if ((err = elf_ctx_create(&module_ctx, (uint8_t*)module_start, module_size)) != ELF_OK) {
            log(ERR, "Failed to load module %s: %s", module->name, elf_strerror(err));
            panic("Unable to load boot modules!");
        }

        if ((err = elf_ctx_load(&module_ctx, &info)) != ELF_OK) {
            log(ERR, "Failed to load module %s: %s", module->name, elf_strerror(err));
            panic("Unable to load boot modules!");
        }

        proc->page_dir     = pd_addr;
        proc->priority     = 0;
        proc->id           = pid++;
        proc->threads      = thread;
        proc->thread_count = 1;

        thread->proc               = proc;
        thread->sched_ctx.priority = 0;
        thread->id                 = 0;

        extern vaddr_t kstack_top;

        // TODO: SMP overhaul point
        native_create_thread(thread, (vaddr_t)module_ctx.elf_hdr->e_entry, (vaddr_t)info.main_thread_stack_top, (vaddr_t)kstack_top);
    }
}

void bootstrap(kernel_info_t* info)
{
    native_disable_interrupts();

    memcpy(&kinfo, info, sizeof(kernel_info_t));

    console_init();

    log(INFO, "Bootstrap started");
    log(INFO, "Initializing kernel memory");
    kmem_init(&kinfo);

    // TODO: map kheap before bootstrapping modules
    bootstrap_identity_map_init_mem(&kinfo);
    load_boot_modules();

    kmem_init_cleanup();

    arch_init();

    extern sched_policy_t sched_rr;

    sched_init(&sched_rr, boot_processes, kinfo.no_modules);

    ktime_init(sched_rr.interrupt_handler);

    arch_return_to_user();
}
