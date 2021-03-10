#pragma once

#include "list.h"
#include <arch/objs.h>
#include <types/base.h>

/*
 * Definitions of all kernel objects
 */

typedef size_t pid_t;
typedef size_t tid_t;

typedef struct __attribute__((packed)) proc {
    pid_t  id;
    regs_t saved_registers;

    vaddr_t stack_base;

    uint8_t   priority;
    lst_hdr_t list;
} proc_t;

typedef struct tcb {

    tid_t id;

    reg_t     ctrl;
    reg_t     stack_ptr;
    reg_t     inst_ptr;
    reg_t     flags;
    lst_hdr_t list;

} tcb_t;
