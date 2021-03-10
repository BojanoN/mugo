#include "time.h"
#include "conf.h"
#include "log.h"

#include <arch/time.h>

extern hwclock_t hwclock;

void ktime_init(void (*timer_callback)(void))
{
    hwclock.hwclock_register_callback(timer_callback);
    hwclock.hwclock_init(CONF_SCHED_TICK_FREQ_HZ);
}

int kclock_gettime(clockid_t clock, timespec_t* time)
{

    return 0;
}
