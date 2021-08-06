#include <meminfo.h>
#include <sys/syscall.h>

int arch_invoke_kcall(void);

int kcall_debug(const char* msg)
{

    asm volatile("mov %0,%%ebx"
                 :
                 : "r"(msg)
                 : "memory");
    asm volatile("mov $0x0, %eax");

    return arch_invoke_kcall();
}

int kcall_meminfo(struct sys_meminfo* meminfo)
{

    asm volatile("mov %0,%%ebx"
                 :
                 : "r"(meminfo)
                 : "memory");
    asm volatile("mov $0x1, %eax");

    return arch_invoke_kcall();
}
