#pragma once
#include <arch_exec.h>
#include <types/base.h>

#define PAGE_ALLOCATE ((paddr_t)0)

typedef struct exec_info exec_info_t;

typedef void (*exec_on_load)(void);
typedef int (*exec_mmap)(exec_info_t*, paddr_t, vaddr_t, size_t, unsigned int);

typedef struct exec_info {
    paddr_t      page_directory;
    vaddr_t      main_thread_stack_top;
    exec_mmap    mmap;
    exec_on_load on_load;
} exec_info_t;
