#include <sys/syscall.h>

int x = 1;

void main(void)
{
    syscall(0, "asdf", 0);
    while (1) {
    }
}
