#include "include/arch_thread.h"

#include "include/arch_util.h"
#include <arch/paging.h>
#include <kern/assert.h>
#include <stdlib.h>

/*
 * Architecture specific thread context routines.
 */

/* Interrupt flag enabled */
#define THREAD_DEFAULT_EFLAGS (0x2 | (1 << 9))

arch_thread_context_t* arch_current_thread_execution_ctx = NULL;
static proc_t*         current_proc                      = NULL;

void native_create_thread(tcb_t* tcb, vaddr_t entrypoint, vaddr_t thread_stack_base, vaddr_t k_stack_base)
{
    arch_thread_context_t* exec_ctx = &tcb->exec_ctx.arch_exec_ctx;

    exec_ctx->esp = (reg_t)(thread_stack_base);
    exec_ctx->eip = (reg_t)entrypoint;

    //tcb->k_stack_base = k_stack_base;
    //    tcb->k_stack_size = k_stack_size;

    // TODO: select valid SS
    exec_ctx->ss = exec_ctx->ds = 0x23;
    exec_ctx->cs                = 0x1B;
    exec_ctx->eflags            = THREAD_DEFAULT_EFLAGS;

    memset(&exec_ctx->gp_regs_state, 0, sizeof(arch_gp_regs));
}

void native_select_thread(tcb_t* tcb)
{
    //ASSERT(tcb->k_stack_base != 0);

    //  native_update_tss(tcb->k_stack_base);
    if (&tcb->exec_ctx.arch_exec_ctx != arch_current_thread_execution_ctx) {
        arch_current_thread_execution_ctx = &tcb->exec_ctx.arch_exec_ctx;
    }

    if (tcb->proc != current_proc) {
        current_proc = tcb->proc;
        arch_load_pagedir(current_proc->page_dir);
    }
}
