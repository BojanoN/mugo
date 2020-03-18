#include <vga.h>
#include <string.h>

unsigned char stack [ 0x100000 ];

const char* msg = "UwU";

void kernel_start(void){
  vga_clear_screen();
  vga_write_string(msg, 0xF, 0x0);
}
