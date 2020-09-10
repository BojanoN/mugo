#include <kern/kprint.h>
#include <string.h>

unsigned char stack[0x10000];

const char* msg = "Hi!\n";

void kernel_start(void)
{
    console_init();
    /*console_write_string(msg);
    console_write_string("Halting..\n");
  */

    kprintf("%s %c\n", msg, '?');
}
