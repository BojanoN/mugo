#include "include/arch_thread.h"

#include "include/arch_util.h"
#include <kern/assert.h>
#include <stdlib.h>

/*
 * Architecture specific thread contex routines.
 */

/* Interrupt flag enabled */
#define THREAD_DEFAULT_EFLAGS (0x2 | (1 << 9))

arch_thread_context_t* arch_current_thread_execution_ctx = NULL;

void native_create_thread(tcb_t* tcb, vaddr_t entrypoint, vaddr_t thread_stack_base, size_t thread_stack_size, vaddr_t k_stack_base, vaddr_t k_stack_size)
{
    arch_thread_context_t* exec_ctx = &tcb->exec_ctx.arch_exec_ctx;

    exec_ctx->esp = (reg_t)(thread_stack_base + thread_stack_size);
    exec_ctx->eip = (reg_t)entrypoint;

    tcb->k_stack_base = k_stack_base;
    tcb->k_stack_size = k_stack_size;

    // TODO: select valid SS
    exec_ctx->ss     = 0x23;
    exec_ctx->cs     = 0x1B;
    exec_ctx->eflags = THREAD_DEFAULT_EFLAGS;

    memset(&exec_ctx->gp_regs_state, 0, sizeof(arch_gp_regs));
}

void native_select_thread(tcb_t* tcb)
{
    ASSERT(tcb->k_stack_base != 0);

    native_update_tss(tcb->k_stack_base + tcb->k_stack_size);
    arch_current_thread_execution_ctx = &tcb->exec_ctx.arch_exec_ctx;
}
