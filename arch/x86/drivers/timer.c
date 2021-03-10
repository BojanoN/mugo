#include <arch/interrupt.h>
#include <arch/time.h>
#include <arch/util.h>
#include <kern/kprint.h>

#include "timer.h"

#define PIT_CMD           0x43
#define PIT_DATA_CHANNEL0 0x40
#define PIT_DATA_CHANNEL1 0x41
#define PIT_DATA_CHANNEL2 0x42

#define PIT_REPEAT_MODE 0x36

#define PIT_FREQ          1193180
#define PIT_MAX_VAL       0xFFFF
#define PIT_NSEC_PER_TICK 838

static void hwclock_register_callback(hwclock_callback_t cb);
static void hwclock_init(uint32_t frequency);
static void hwclock_get_time(timespec_t* tv);
static void hwclock_set_time(timespec_t* tv);

//static hwclock_stats_t    timer_stats      = { 0 };
static hwclock_callback_t current_callback = NULL;

static size_t current_freq_hz = 0;
static size_t current_ctr_val = 0;

static timespec_t current_clock = { 0 };

hwclock_t hwclock = {
    hwclock_register_callback,
    hwclock_init,
    hwclock_get_time,
    hwclock_set_time,
};

//static void hwclock_set_tick_duration(time_t t_nsec)
//{
//}

static time_t hwclock_get_tick_remaining(void)
{
    uint8_t low, hi;

    outb(PIT_CMD, PIT_REPEAT_MODE);
    low = inb(PIT_DATA_CHANNEL0);
    hi  = inb(PIT_DATA_CHANNEL0);

    return (low | (hi << 8)) * PIT_NSEC_PER_TICK;
}
/*
static void hwclock_stub_callback(void)
{
    timer_stats.total_ticks++;
    kprintf("Total ticks: %d\n", timer_stats.total_ticks);
    timespec_t clock;

    hwclock_get_time(&clock);
    kprintf("Current clock: %d, %d\n", clock.tv_sec, clock.tv_nsec);
}*/

static void hwclock_irq_handler(irq_context_t ctx)
{
    time_t tmp_nsec = current_clock.tv_nsec + current_ctr_val * PIT_NSEC_PER_TICK;

    if (tmp_nsec > 1000000000) {
        tmp_nsec -= 1000000000;
        current_clock.tv_sec++;
    }

    current_clock.tv_nsec = tmp_nsec;

    current_callback();
}

static void hwclock_register_callback(hwclock_callback_t cb)
{
    current_callback = cb;
}

static void hwclock_init(uint32_t frequency)
{

    register_interrupt_callback(IRQ0, hwclock_irq_handler);

    uint32_t d = PIT_FREQ / frequency;

    current_freq_hz = frequency;
    current_ctr_val = d;

    outb(PIT_CMD, PIT_REPEAT_MODE);

    outb(PIT_DATA_CHANNEL0, (uint8_t)(d & 0xFF));
    outb(PIT_DATA_CHANNEL0, (uint8_t)(d >> 8));

    arch_unmask_irq(IRQ0);
}

static void hwclock_set_time(timespec_t* tv)
{
    current_clock = *tv;
}

static void hwclock_get_time(timespec_t* tv)
{
    tv->tv_sec  = current_clock.tv_sec;
    tv->tv_nsec = current_clock.tv_nsec;

    time_t remainder = hwclock_get_tick_remaining();
    time_t tmp_nsec  = tv->tv_nsec + remainder;

    if (tmp_nsec > 1000000000) {
        tmp_nsec -= 1000000000;
        tv->tv_sec++;
    }

    tv->tv_nsec = tmp_nsec;
}
