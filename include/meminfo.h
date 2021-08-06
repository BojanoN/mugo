#pragma once

#include <types/base.h>

#define NO_MEM_REGIONS 20

typedef struct {
    paddr_t addr;
    size_t  len;
} mem_region_t;

struct sys_meminfo {
    mem_region_t regions[NO_MEM_REGIONS];
    size_t       no_mem_regions;
};
