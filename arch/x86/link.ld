ENTRY(_start)

K_BOOT_VMA = 0x00100000;
K_HIGH_VMA = 0xC0000000;

SECTIONS {

  . = K_BOOT_VMA;

  .boot.data :{
           *(.multiboot)
           *(.boot.data)
  }

  .boot.text ALIGN(4K) : {
           *(.boot.text)
           *paging.s(.text)
  }


  . += K_HIGH_VMA;
  __kernel_start = .;

  .text ALIGN(4K) : AT(ADDR (.text) - K_HIGH_VMA) {
      *(.text)
  }

  .bss ALIGN(4K) : AT(ADDR (.bss) - K_HIGH_VMA){
     *(.bss)
     *(.boot.bss)
  }

  .rodata ALIGN(4K) : AT(ADDR (.rodata) - K_HIGH_VMA){
     *(.rodata)
  }

  .data ALIGN(4K) : AT(ADDR (.data) - K_HIGH_VMA){
      *(.data)
  }

  __kernel_end = .;
}
