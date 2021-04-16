#pragma once

#include <types/base.h>

typedef struct {
    paddr_t start;
    paddr_t end;
} phys_mem_range_t;

typedef struct {
    vaddr_t start;
    vaddr_t end;
} mem_range_t;
