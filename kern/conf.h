#pragma once

#include <arch_conf.h>
#include <types/base.h>

extern size_t K_HEAP_SIZE;

#define CONF_KSTACK_SIZE        0x2000
#define CONF_SCHED_TICK_FREQ_HZ 100
#define CONF_KHEAP_SIZE         0x1000000

#define CONF_SCHED_RR_DRIVER_QUANT_MSEC 10
#define CONF_SCHED_RR_SERVER_QUANT_MSEC 5
#define CONF_SCHED_RR_USER_QUANT_MSEC   1
#define CONF_MAX_BOOT_MODULES           8
