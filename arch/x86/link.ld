ENTRY(_start)

K_BOOT_VMA = 0x00100000;
K_HIGH_VMA = 0xC0000000;

K_HEAP_VMA = 0xD0000000;
K_HEAP_SIZE = 0x100000;
K_DRIVERS_VMA = 0xE0000000;
K_METADATA_VMA = 0xF0000000;
K_METADATA_SIZE =  0x80000;

SECTIONS {

  . = K_BOOT_VMA;
  __early_start = .;

  .boot.data :{
           *(.multiboot)
           *(.boot.data)
  }

  .boot.pt ALIGN(4K) :{
          *(.boot.pt)
  }

  .boot.text ALIGN(4K) : {
           *(.boot.text)
           *paging.s(.text)
  }
  __early_end = .;
  . = ALIGN(4K);



 

  . = __early_end + K_HIGH_VMA;
  __ktable_start = .;
  .ktable ALIGN(4K) : AT(ADDR (.ktable) - K_HIGH_VMA) {
  *(.ktable)
  }

  __kernel_start = .;

  .text ALIGN(4K) : AT(ADDR (.text) - K_HIGH_VMA) {
      *(.text)
  }
  __kernel_text_end = .;

  .bss ALIGN(4K) : AT(ADDR (.bss) - K_HIGH_VMA){
     *(.bss)
  }

  __kernel_bss_end = .;
  .rodata ALIGN(4K) : AT(ADDR (.rodata) - K_HIGH_VMA){
     *(.rodata)
  }

  __kernel_rodata_end = .;
  .data ALIGN(4K) : AT(ADDR (.data) - K_HIGH_VMA){
      *(.data)
  }

  /* stack guard page */
   . = ALIGN(4K);
   __kernel_stack_guard_start = .;

   . += 4K;
  .stack  ALIGN(4K) : AT(ADDR (.stack) - K_HIGH_VMA){
        *(.stack)
  }

  . = ALIGN(4K);
  __kernel_stack_end = .;

  . += 4K; /* heap guard page */
  __kernel_heap_start = .;
  . += K_HEAP_SIZE;

  __kernel_end = .;


  __metadata_start = . - K_HIGH_VMA;
  __metadata_end = __metadata_start + K_METADATA_SIZE;

  . = ALIGN(4K);
  __next_phys = . - K_HIGH_VMA;
. = 0xa0000000;
__usr_test_start = .;
.usr.text ALIGN(4K) : AT(__next_phys){
*(.usr.txt)
}
.usr.stack ALIGN(4K) : AT( __next_phys + 4096){
*(.usr.stk)
}

}
