#define VGA_BUF_ADDR 0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

unsigned char stack [ 0x1000 ];

const char* msg = "UwU";
unsigned char* term_buff = (unsigned char*) VGA_BUF_ADDR;

unsigned int strlen(const char* str){
  unsigned int len = 0;
  while(str[len])
    len++;
  return len;
}

void clear_screen(){
  char clr = ' ';
 
  for(unsigned int i=0; i < VGA_HEIGHT; i++){
    for(unsigned int j=0; j < VGA_WIDTH; j++){

      term_buff[(i * VGA_HEIGHT) + j] = clr;
      term_buff[(i * VGA_HEIGHT) + j + 1] = 0;

    }
  }
}

void write_string(const char* str, unsigned char forecolor, unsigned char backcolor){
  unsigned char attrib = (backcolor << 4) | (forecolor);
  int x = 0;
  int y = 0;

  unsigned int n = strlen(str);

  for(unsigned int i=0; i < n; i++){
    
    term_buff[(y * VGA_HEIGHT) + x] = str[i];
    term_buff[(y * VGA_HEIGHT) + x + 1] = attrib;


    x++;
    if(x >= VGA_WIDTH){
      x = 0;
      y++;
    }
  }

}

void kernel_start(void){
  clear_screen();
  write_string(msg, 0x2, 0x1);
}
