#include <meminfo.h>
#include <sys/syscall.h>

static struct sys_meminfo meminfo = { 0 };

void main(void)
{
    int err;

    kcall_debug("TEST");

    err = kcall_meminfo(&meminfo);

    while (1) {
    }
}
