#pragma once

#include <arch/interrupt.h>
#include <kern/kobjs.h>

void native_create_thread(tcb_t* tcb, vaddr_t entrypoint, vaddr_t thread_stack_base, vaddr_t k_stack_base);
void native_select_thread(tcb_t* tcb);
void arch_update_exec_ctx(irq_context_t* irq_ctx);
