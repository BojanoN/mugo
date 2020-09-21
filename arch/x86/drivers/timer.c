#include "timer.h"
#include "../util.h"
#include <arch/interrupt.h>
#include <kern/kprint.h>

#define PIT_CMD           0x43
#define PIT_DATA_CHANNEL0 0x40
#define PIT_DATA_CHANNEL1 0x41
#define PIT_DATA_CHANNEL2 0x42

#define PIT_REPEAT_MODE 0x36

#define PIT_FREQ 1193180

static timer_stats_t timer_stats = { 0 };

static void timer_irq_handler(irq_context_t ctx)
{
    timer_stats.total_ticks++;
    kprintf("Total ticks: %d\n", timer_stats.total_ticks);
}

void init_timer(uint32_t frequency)
{
    register_interrupt_callback(IRQ0, &timer_irq_handler);

    uint32_t d = PIT_FREQ / frequency;

    outb(PIT_CMD, PIT_REPEAT_MODE);

    outb(PIT_DATA_CHANNEL0, (uint8_t)(d & 0xFF));
    outb(PIT_DATA_CHANNEL0, (uint8_t)((d >> 8) & 0xFF));

    arch_unmask_irq(IRQ0);
}
