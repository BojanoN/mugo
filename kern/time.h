#pragma once

#include <arch/interrupt.h>
#include <types/time.h>

void ktime_init(void (*timer_callback)(irq_context_t*));
int  kclock_gettime(clockid_t clock, timespec_t* time);

static inline void time_sub_nsec(time_t* tv_nsec, time_t nsec)
{
    *tv_nsec = *tv_nsec > nsec ? *tv_nsec - nsec : 0;
}
