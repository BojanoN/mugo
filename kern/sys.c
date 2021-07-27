#include <arch/info.h>
#include <stdlib.h>

#include "assert.h"
#include "kobjs.h"
#include "kprint.h"
#include "sched.h"
#include "sys.h"
#include "util.h"

extern sched_policy_t scheduler;

static kernel_info_t* kinfo = NULL;

static inline void check_param(void* param)
{
    if (!param || (vaddr_t)param >= kinfo->kernel_high_vma) {
        panic("Invalid parameter passed to syscall");
    }
}

static int sys__kinfo(void* dst_buf)
{
    check_param(dst_buf);

    memcpy(dst_buf, (void*)kinfo, sizeof(kernel_info_t));

    return 0;
}

static int sys__dbg_print(void* str)
{
    check_param(str);

    kprintf("%s", (char*)str);
    return 0;
}

static syscall_t syscalls[] = {
    NULL,
    sys__dbg_print,
    sys__kinfo
};

void syscall_init(kernel_info_t* info)
{
    kinfo = info;
}

static size_t no_syscalls = sizeof(syscalls) / sizeof(syscall_t);

void syscall_handler(void)
{
    tcb_t* current_thread = scheduler.current_thread();
    ASSERT(current_thread);
    utcb_t* user_tcb = (utcb_t*)current_thread->utcb_ptr;
    ASSERT(user_tcb);
    // TODO: handle IPC arguments stored in the UTCB
    // save new EIP and ESP should a new thread be scheduled
    size_t invoked_syscall_num = user_tcb->syscall_num;

    if (invoked_syscall_num < no_syscalls && syscalls[invoked_syscall_num]) {
        syscalls[invoked_syscall_num](user_tcb->syscall_params);
    }
}
