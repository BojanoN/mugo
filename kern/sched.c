#include "sched.h"
#include "assert.h"

sched_policy_t current_scheduler = { 0 };

void sched_init(sched_policy_t* sched, tcb_t* init)
{
    ASSERT(sched != NULL);
    ASSERT(sched->init != NULL);
    ASSERT(sched->interrupt_handler != NULL);
    ASSERT(sched->schedule != NULL);
    ASSERT(sched->idle != NULL);
    ASSERT(sched->current_thread != NULL);

    current_scheduler = *sched;

    current_scheduler.init(init);
}
