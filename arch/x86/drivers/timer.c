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

#define PIT_FREQ    1193180
#define PIT_MAX_VAL 0xFFFF

static void register_callback(timer_callback_t cb);
static void timer_set_tick_duration(timespec_t* tv);
static void timer_get_tick_remaining(timespec_t* tv);

static timer_stats_t    timer_stats      = { 0 };
static timer_callback_t current_callback = NULL;

timer_t arch_timer = {
    register_callback,
    init_timer,
    timer_set_tick_duration,
    timer_get_tick_remaining
};

static void stub_callback(void)
{
    timer_stats.total_ticks++;
    kprintf("Total ticks: %d\n", timer_stats.total_ticks);
}

static void timer_irq_handler(irq_context_t ctx)
{
    current_callback();
}

static void timer_set_tick_duration(timespec_t* tv)
{
}

static void timer_get_tick_remaining(timespec_t* tv)
{
}

static void register_callback(timer_callback_t cb)
{
    current_callback = cb;
}

void init_timer(uint32_t frequency)
{
    register_callback(stub_callback);
    register_interrupt_callback(IRQ0, timer_irq_handler);

    uint32_t d = PIT_FREQ / frequency;

    outb(PIT_CMD, PIT_REPEAT_MODE);

    outb(PIT_DATA_CHANNEL0, (uint8_t)(d & 0xFF));
    outb(PIT_DATA_CHANNEL0, (uint8_t)(d >> 8));

    arch_unmask_irq(IRQ0);
}
