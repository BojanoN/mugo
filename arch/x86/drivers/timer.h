#pragma once
#include <types.h>

void init_timer(uint32_t frequency);

typedef void (*timer_callback)(void);

typedef struct {
    uint64_t total_ticks;
} timer_stats_t;
