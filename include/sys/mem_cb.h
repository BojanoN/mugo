#pragma once
#include <types/base.h>

typedef struct {
    int (*mmap)(vaddr_t, size_t, unsigned int);

} mem_callback_t;
