
#include <types/base.h>

#include <kern/assert.h>
#include <kern/kcall.h>
#include <kern/kobjs.h>
#include <kern/kprint.h>
#include <kern/sched.h>
#include <sys/cdefs.h>

extern sched_policy_t scheduler;
extern vaddr_t        K_HIGH_VMA;

struct debug_args {
};

static int debug(tcb_t* td, struct debug_args* args)
{
    kprintf("kcall test");

    return 0;
}

static vaddr_t kcall_table[] = {
    (vaddr_t)&debug
};

int kcall_handler(kcall_frame_t* frame)
{

    int retval = 0;

    if (unlikely(frame->kcall_number > NO_KCALLS && frame->kcall_number < 0)) {
        return -1;
    }

    vaddr_t kcall = kcall_table[frame->kcall_number];

    tcb_t* current_thread = scheduler.current_thread();
    ASSERT_MSG(current_thread, "current thread is NULL");

    retval = ((kcall_t)kcall)(current_thread, frame);

    return retval;
}
