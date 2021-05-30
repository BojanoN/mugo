#pragma once

#include "list.h"
#include <arch/objs.h>
#include <types/base.h>
#include <types/time.h>

/*
 * Definitions of all kernel objects
 */

#define NR_TOKENS_PER_THREAD 4

typedef size_t pid_t;
typedef size_t tid_t;

// Capability struct
typedef struct token {

} token_t;

// Resides in userspace at a fixed address space per process
typedef struct utcb {

} utcb_t;

typedef struct tcb_exec_ctx {

    arch_thread_context_t arch_exec_ctx;

} tcb_exec_ctx_t;

typedef struct tcb_sched_ctx {
    size_t priority;
    size_t state;

    time_t quantum_nsec;
} tcb_sched_ctx_t;

struct proc;

typedef struct tcb {
    tid_t        id;
    struct proc* proc;

    tcb_sched_ctx_t sched_ctx;
    tcb_exec_ctx_t  exec_ctx;

    /* Each thread uses a per-CPU kernel stack  */
    vaddr_t utcb_ptr;

    lst_hdr_t list;
} tcb_t;

typedef struct proc {
    pid_t id;

    paddr_t page_dir;

    size_t priority;

    tcb_t* threads;
    size_t thread_count;

    lst_hdr_t list;
} proc_t;
