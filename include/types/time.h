#pragma once

#include <types/base.h>

#define CLOCK_REALTIME  1
#define CLOCK_MONOTONIC 2

#define SEC_NSEC 1000000000

typedef uint64_t time_t;
typedef size_t   clockid_t;

typedef struct timespec {
    time_t tv_sec;
    long   tv_nsec;
} timespec_t;

typedef struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
} itimerspec_t;

typedef size_t timer_t;
