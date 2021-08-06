#include <types/base.h>

#include <arch/info.h>
#include <kern/assert.h>
#include <kern/kcall.h>
#include <kern/kobjs.h>
#include <kern/kprint.h>
#include <kern/sched.h>
#include <sys/cdefs.h>

#include <meminfo.h>
#include <proc.h>

#include "log.h"

extern sched_policy_t scheduler;
extern vaddr_t        K_HIGH_VMA;

static inline int __validate(vaddr_t usr_ptr)
{
    return unlikely(usr_ptr < (vaddr_t)&K_HIGH_VMA);
}

#define VALIDATE_USR_PTR(arg)                  \
    do {                                       \
        if (__validate((vaddr_t)(arg)) == 0) { \
            return -1;                         \
        }                                      \
    } while (0)

struct debug_args {
    const char* msg;
};

struct meminfo_args {
    struct sys_meminfo* meminfo;
};

static int debug(tcb_t* td, struct debug_args* args)
{
    VALIDATE_USR_PTR(args->msg);

    return log(DEBUG, args->msg);
}

static int meminfo(tcb_t* td, struct meminfo_args* args)
{
    VALIDATE_USR_PTR(args->meminfo);

    proc_t* proc = td->proc;

    if (proc->id != PROC_MMAN_PID) {
        return -1;
    }

    extern kernel_info_t kinfo;

    args->meminfo->no_mem_regions = kinfo.no_mmaps;
    for (size_t i = 0; i < kinfo.no_mmaps; i++) {
        args->meminfo->regions[i].addr = kinfo.memmaps[i].addr;
        args->meminfo->regions[i].len  = kinfo.memmaps[i].len;
    }

    return 0;
}

static vaddr_t kcall_table[] = {
    (vaddr_t)&debug,
    (vaddr_t)&meminfo,
};

int kcall_handler(kcall_frame_t* frame)
{
    int retval = 0;

    if (unlikely(frame->kcall_number > NO_KCALLS || frame->kcall_number < 0)) {
        return -1;
    }

    vaddr_t kcall = kcall_table[frame->kcall_number];
    ASSERT_MSG(kcall, "kcall not implemented");

    tcb_t* current_thread = scheduler.current_thread();
    ASSERT_MSG(current_thread, "current thread is NULL");

    retval = ((kcall_t)kcall)(current_thread, frame->args);

    return retval;
}
