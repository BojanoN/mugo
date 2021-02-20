#pragma once

#include <types/time.h>

void ktime_init(void);
int  kclock_gettime(clockid_t clock, timespec_t* time);
