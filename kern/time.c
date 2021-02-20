#include "time.h"
#include "conf.h"

#include <arch/time.h>

extern hwclock_t hwclock;

static void tick_handler(void);

void ktime_init(void)
{
    hwclock.hwclock_register_callback(tick_handler);
    hwclock.hwclock_init(CONF_SCHED_TICK_FREQ_HZ);
}

static void tick_handler(void)
{
}

int kclock_gettime(clockid_t clock, timespec_t* time)
{

    return 0;
}
