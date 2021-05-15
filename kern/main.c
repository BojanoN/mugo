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

extern sched_policy_t sched_rr;
kernel_info_t         kinfo;
//static tcb_t          init_tcb;

void arch_init(void);
void arch_return_to_user(void);

static proc_t boot_processes[CONF_MAX_BOOT_MODULES] = { 0 };

void load_boot_modules(void)
{
    size_t              no_modules = kinfo.no_modules;
    elf_ctx_t           module_ctx;
    int                 err;
    extern page_table_t kernel_pt;
    extern page_table_t bootstrap_pt;

    // identity map the necessary memory range for easier initialization

    ASSERT_MSG(no_modules < CONF_MAX_BOOT_MODULES, "Too much boot modules loaded");

    memset(&module_ctx, 0, sizeof(elf_ctx_t));
    exec_info_t info = { 0 };
    info.mmap        = bootstrap_mmap_elf;

    paddr_t bootstrap_pt_phys   = (vaddr_t)&bootstrap_pt - (paddr_t)kinfo.kernel_high_vma;
    size_t  bootstrap_pt_offset = arch_get_pt_offset(kinfo.bootstrap_memory_phys_range.start, ARCH_PD_1);
    size_t  kernel_pt_offset    = arch_get_pt_offset(kinfo.kernel_phys_range.start + kinfo.kernel_high_vma, ARCH_PD_1);
    paddr_t kernel_pt_phys      = (((vaddr_t)&kernel_pt - (vaddr_t)kinfo.kernel_high_vma) & PAGE_MASK);

    for (size_t i = 0; i < no_modules; i++) {

        struct boot_module* module       = &kinfo.modules[i];
        vaddr_t             module_start = module->start_paddr;
        size_t              module_size  = module->end_paddr - module->start_paddr;
        proc_t*             proc         = &boot_processes[i];

        paddr_t pd_addr = bootstrap_create_page_dir();
        if (pd_addr == 0) {
            panic("Unable to create page directory for module");
        }

        memset((void*)pd_addr, 0, ARCH_PAGE_SIZE);
        info.page_directory = pd_addr;
        proc->page_dir      = pd_addr;

        // Create paging structures, map kernel
        page_directory_t* module_pd             = (page_directory_t*)pd_addr;
        module_pd->entries[kernel_pt_offset]    = kernel_pt_phys | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);
        module_pd->entries[bootstrap_pt_offset] = bootstrap_pt_phys | (ARCH_PD_ENTRY_PRESENT | ARCH_PD_ENTRY_RW);

        arch_load_pagetable(pd_addr);

        log(INFO, "Loading module %s...", module->name);

        if ((err = elf_ctx_create(&module_ctx, (uint8_t*)module_start, module_size)) != ELF_OK) {
            log(ERR, "Failed to load module %s: %s", module->name, elf_strerror(err));
            panic("Unable to load boot modules!");
        }

        if ((err = elf_ctx_load(&module_ctx, &info)) != ELF_OK) {
            log(ERR, "Failed to load module %s: %s", module->name, elf_strerror(err));
            panic("Unable to load boot modules!");
        }

        // TODO: assign one page stack
        proc->stack_base = CONF_PROCESS_STACK_BASE;
    }
}

void bootstrap(kernel_info_t* info)
{
    native_disable_interrupts();

    memcpy(&kinfo, info, sizeof(kernel_info_t));

    console_init();

    log(INFO, "Bootstrap started");

    log(INFO, "Initializing kernel memory");
    map_kernel_memory(&kinfo);

    bootstrap_identity_map_init_mem(&kinfo);

    load_boot_modules();

    kmem_init(&kinfo);

    arch_init();

    //   memset(&init_tcb, 0, sizeof(tcb_t));

    //    init_tcb.id = 0;

    //native_create_thread(&init_tcb, (vaddr_t)test_func, (vaddr_t)test_stack, 0x1000, (vaddr_t)test_kstack, 0x1000);

    // sched_init(&sched_rr, &init_tcb);

    //ktime_init(sched_rr.interrupt_handler);

    //arch_return_to_user();
}
