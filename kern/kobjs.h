#pragma once

#include <arch/objs.h>
#include <types/base.h>

/*
 * Definitions of all kernel objects
 */

typedef size_t pid_t;

typedef struct __attribute__((packed)) proc {
    pid_t  id;
    regs_t saved_registers;

    vaddr_t stack_base;

    uint8_t priority;
} proc_t;
