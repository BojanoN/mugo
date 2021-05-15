ENTRY(_start)

K_BOOT_VMA = 0x00100000;
K_HIGH_VMA = 0xFE800000; /*0xC0000000; */


SECTIONS {

  . = K_BOOT_VMA;
  __early_start = .;

  . = ALIGN(8);
  .boot.data :{
           KEEP(*(.multiboot))
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





  . += K_HIGH_VMA;
  . = ALIGN(4K);

  __kernel_start = .;
  __ktable_start = .;
  .ktable ALIGN(4K) : AT(ADDR (.ktable) - K_HIGH_VMA) {
  *(.ktable)
  }
  . = ALIGN(4K);
  __ktable_end = .;


  .text ALIGN(4K) : AT(ADDR (.text) - K_HIGH_VMA) {
      *(.text)
  }
  . = ALIGN(4K);
  __kernel_text_end = .;

  .bss ALIGN(4K) : AT(ADDR (.bss) - K_HIGH_VMA){
     *(.bss)
  }

. = ALIGN(4K);
  __kernel_bss_end = .;
  .rodata ALIGN(4K) : AT(ADDR (.rodata) - K_HIGH_VMA){
     *(.rodata*)
  }

. = ALIGN(4K);
  __kernel_rodata_end = .;
  .data ALIGN(4K) : AT(ADDR (.data) - K_HIGH_VMA){
      *(.data)
  }

  /* stack guard page */
   . = ALIGN(4K);
   __kernel_stack_guard_start = .;

   . += 4K;
   __kernel_stack_start = .;

  .stack  ALIGN(4K) : AT(ADDR (.stack) - K_HIGH_VMA){
        *(.stack)
  }

  . = ALIGN(4K);
  __kernel_stack_end = .;

  __kernel_end = .;

  __console_debug = .;
  . += 4K;

}
