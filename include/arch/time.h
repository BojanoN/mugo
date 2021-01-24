#pragma once

//#include <arch_time.h>

#include <types.h>

typedef unsigned long time_t;

typedef struct timespec {
    time_t tv_sec;
    long   tv_nsec;
} timespec_t;

typedef struct {
    uint64_t total_ticks;
} timer_stats_t;

typedef void (*timer_callback_t)(void);

typedef struct timer {
    void (*timer_register_callback)(timer_callback_t);
    void (*timer_init)(uint32_t);

    void (*timer_set_tick_duration)(timespec_t*);
    void (*timer_get_tick_remaining)(timespec_t*);
} timer_t;
