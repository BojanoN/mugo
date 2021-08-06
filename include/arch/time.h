#pragma once

#include <arch/interrupt.h>
#include <types/time.h>

typedef struct {
    uint64_t total_ticks;
} hwclock_stats_t;

typedef void (*hwclock_callback_t)(irq_context_t*);

typedef struct hwclock {
    void (*hwclock_register_callback)(hwclock_callback_t);
    void (*hwclock_init)(uint32_t);

    void (*hwclock_set_time)(timespec_t*);
    void (*hwclock_get_time)(timespec_t*);
} hwclock_t;
