#include <vga.h>
#include <string.h>

static const unsigned int VGA_WIDTH = 80 * 2;
static const unsigned int VGA_HEIGHT = 25 * 2;


static unsigned char* term_buff = (unsigned char*) 0xB8000;

void vga_clear_screen(){
  char clr = ' ';

  for(unsigned int i=0; i < VGA_HEIGHT; i++){
    for(unsigned int j=0; j < VGA_WIDTH ; j+=2){

      term_buff[(i * VGA_HEIGHT) + j] = clr;
      term_buff[(i * VGA_HEIGHT) + j + 1] = 0;

    }
  }
}

void vga_write_string(const char* str, unsigned char forecolor, unsigned char backcolor){
  unsigned char attrib = (backcolor << 4) | (forecolor);
  int x = 0;
  int y = 0;

  unsigned int n = strlen(str);

  for(unsigned int i=0; i < n; i++, x+=2){
    term_buff[(y * VGA_HEIGHT) + x] = str[i];
    term_buff[(y * VGA_HEIGHT) + x + 1] = attrib;

    if(x >= VGA_WIDTH){
      x = 0;
      y++;
    }
  }
}
