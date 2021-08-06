#include "log.h"
#include "sched.h"
#include "time.h"

#include <arch/thread.h>
#include <kern/assert.h>

/*
 * RR scheduler with varying quantum sizes based on priority
 */

#define READY_QUEUE_LEN 16

static void   rr_tick_handler(irq_context_t*);
static void   rr_init(proc_t*, size_t);
static void   rr_schedule(void);
static void   rr_idle(void);
static tcb_t* rr_current_thread(void);

sched_policy_t sched_rr = {
    rr_init,
    rr_tick_handler,
    rr_schedule,
    rr_idle,
    rr_current_thread
};

static tcb_t ready_queue[READY_QUEUE_LEN] = { 0 };
// TODO: set to IDLE

static tcb_t*           current_thread    = NULL;
static tcb_sched_ctx_t* current_sched_ctx = NULL;

static long nsec_to_next_tick = 0;

static inline void rr_move_to_ready(tcb_t* tcb)
{
    list_add(&tcb->list, ready_queue[tcb->sched_ctx.priority].list.prev);
    tcb->sched_ctx.state = THREAD_READY;
}

static inline time_t rr_quant_from_prio(size_t priority)
{
    // TODO
    return (time_t)10 * SEC_NSEC;
}

static inline tcb_t* rr_get_next_ready(void)
{
    tcb_t* ret = NULL;

    for (int i = 0; i < READY_QUEUE_LEN; i++) {
        if (!list_is_empty(&ready_queue[i].list)) {
            ret = list_data(list_next(&ready_queue[i].list), tcb_t, list);
            break;
        }
    }

    return ret;
}

static void rr_enqueue(tcb_t* thread)
{
    size_t priority                = thread->sched_ctx.priority;
    thread->sched_ctx.quantum_nsec = rr_quant_from_prio(priority);

    rr_move_to_ready(thread);
}

static void rr_tick_handler(irq_context_t* irq_ctx)
{
    log(DEBUG, "schedRR tick");

    // TODO: handle NULL
    if (current_thread) {
        time_sub_nsec(&current_thread->sched_ctx.quantum_nsec, nsec_to_next_tick ? nsec_to_next_tick : sched_tick_nsec());
        arch_update_exec_ctx(irq_ctx);
        rr_schedule();
    }

    // return_to_current()
}

static void rr_init(proc_t* root_tasks, size_t no_tasks)
{
    log(INFO, "Initializing RR scheduler");

    for (size_t i = 0; i < READY_QUEUE_LEN; i++) {
        LIST_HEAD_INIT(&ready_queue[i].list);
    }

    for (size_t i = 0; i < no_tasks; i++) {
        rr_enqueue(root_tasks[i].threads);
    }

    current_thread = rr_get_next_ready();
    native_select_thread(current_thread);
}

static void rr_schedule(void)
{

    tcb_t* next = NULL;

    next = current_thread;

    if (current_thread->sched_ctx.quantum_nsec == 0) {
        rr_move_to_ready(current_thread);
        next = rr_get_next_ready();
    }

    ASSERT(next != NULL);

    if (next != current_thread) {
        current_thread    = next;
        current_sched_ctx = &next->sched_ctx;

        native_select_thread(current_thread);
    }
}

static void rr_idle(void)
{
}

static tcb_t* rr_current_thread(void)
{
    return current_thread;
}
