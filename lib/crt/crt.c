#include <arch_crt.h>
#include <sys/cdefs.h>

stack_canary_t __stack_chk_guard = ARCH_STACK_CHK_GUARD;

NORETURN void __stack_chk_fail(void)
{

#ifdef __MUGO_KERNEL__
#include <kern/util.h>

    panic("Kernel stack smashing detected!");

#endif

    UNREACHABLE;
}
