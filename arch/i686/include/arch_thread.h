#pragma once

#include <kern/kobjs.h>

void native_create_thread(tcb_t* tcb, vaddr_t entrypoint, vaddr_t thread_stack_base, size_t thread_stack_size, vaddr_t k_stack_base, vaddr_t k_stack_size);
void native_sysexit_to_user(vaddr_t instr_ptr, vaddr_t stack_ptr);
void native_iret_to_user(tcb_t* tcb);
void native_select_thread(tcb_t* tcb);
