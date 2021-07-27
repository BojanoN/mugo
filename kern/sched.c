#include "sched.h"
#include "assert.h"

sched_policy_t scheduler = { 0 };

void sched_init(sched_policy_t* sched, proc_t* initial_tasks, size_t no_tasks)
{
    ASSERT(sched != NULL);
    ASSERT(sched->init != NULL);
    ASSERT(sched->interrupt_handler != NULL);
    ASSERT(sched->schedule != NULL);
    ASSERT(sched->idle != NULL);
    ASSERT(sched->current_thread != NULL);

    scheduler = *sched;

    scheduler.init(initial_tasks, no_tasks);
}
