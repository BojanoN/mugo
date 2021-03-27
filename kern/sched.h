#pragma once

#include "conf.h"
#include "kobjs.h"
#include <types/base.h>
#include <types/time.h>

/*
 * Thread state flags
 */

#define THREAD_ACTIVE    0x01
#define THREAD_READY     0x02
#define THREAD_SENDING   0x04
#define THREAD_RECEIVING 0x08

#define sched_thread_is_active(tcb_ptr) \
    ((tcb_ptr)->sched_ctx.state == THREAD_ACTIVE)

static inline time_t sched_tick_nsec(void)
{
    return (time_t)((1. / (float)CONF_SCHED_TICK_FREQ_HZ) * SEC_NSEC);
}

typedef struct sched_policy {
    void (*init)(tcb_t*);
    void (*interrupt_handler)(void);
    void (*schedule)(void);
    void (*idle)(void);
    tcb_t* (*current_thread)(void);
} sched_policy_t;

void sched_init(sched_policy_t* sched, tcb_t* initial_task);
