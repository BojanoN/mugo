#include <arch_crt.h>
#include <sys/cdefs.h>

#ifdef __MUGO_KERNEL__
#include<kern/util.h>

#endif 

stack_canary_t __stack_chk_guard = ARCH_STACK_CHK_GUARD;

NORETURN void __stack_chk_fail(void)
{

#ifdef __MUGO_KERNEL__
    
    panic("Kernel stack smashing detected!");

#endif

    UNREACHABLE;
}
