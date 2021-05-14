#pragma once
#include <types/base.h>

#define PAGE_ALLOCATE ((paddr_t)0)

typedef struct exec_info exec_info_t;

typedef int (*exec_mmap)(exec_info_t*, paddr_t, vaddr_t, size_t, unsigned int);

typedef struct exec_info {
    paddr_t   page_directory;
    exec_mmap mmap;
} exec_info_t;
