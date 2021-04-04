ENTRY(main)

MODULE_VADDR = 0xa0000000;

SECTIONS {

  . = ALIGN(4K);

  .text ALIGN(4K) : AT(ADDR (.text) - MODULE_VADDR) {
      *(.text)
  }

  .bss ALIGN(4K) : AT(ADDR (.bss) - MODULE_VADDR){
     *(.bss)
  }


  .rodata ALIGN(4K) : AT(ADDR (.rodata) - MODULE_VADDR){
      *(.rodata)
  }

  .data ALIGN(4K) : AT(ADDR (.data) - MODULE_VADDR){
      *(.data)
  }

  /DISCARD/ : { *(*) }

}
