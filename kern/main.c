#include <console.h>
#include <string.h>

unsigned char stack[0x100000];

const char* msg = "Hi!\n";

void kernel_start(void)
{
    console_clear_screen();
    console_write_string(msg);
    console_write_string("Halting..");
}
