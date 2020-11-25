ENTRY(_start)

K_BOOT_VMA = 0x00100000;
K_HIGH_VMA = 0xC0000000;

K_HEAP_VMA = 0xD0000000;
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

  .boot.text ALIGN(4K) : {
           *(.boot.text)
           *paging.s(.text)
  }
  __early_end = .;


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

__metadata_start = . - K_HIGH_VMA;
__metadata_end = __metadata_start + K_METADATA_SIZE;

}
