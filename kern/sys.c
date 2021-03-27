#include "sys.h"
#include "kobjs.h"
#include "sched.h"

extern sched_policy_t policy;

void kcall_handler(vaddr_t* stack_ptr, vaddr_t* instr_ptr)
{
    //    tcb_t* current_thread = policy.current_thread();
    // TODO: handle IPC arguments stored in the UTCB
    // save new EIP and ESP should a new thread be scheduled
}
