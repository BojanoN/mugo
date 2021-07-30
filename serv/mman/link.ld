ENTRY(main)

START = 0x400000;

PHDRS {
  text PT_LOAD;
  data PT_LOAD;
  rodata PT_LOAD;
}

SECTIONS {

  .text START : {
      *(.text)
  } :text

  .bss ALIGN(4K) : {
     *(.bss)
  } :data


  .rodata ALIGN(4K) : {
      *(.rodata)
      *(.rodata.*)
  } :rodata

  .data ALIGN(4K) : {
      *(.data)
  } :data

  .got : {
      *(.got*)
  } :data

/* /DISCARD/ : { *(*) }*/
}
