ENTRY(arch_start)

SECTIONS {
         . = 1M;

         .boot :{
               *(.multiboot)
               *(.arch_start)
         }

         .text :{
               *(.text)
         }

         .bss :{
               *(.bss)
         }

         .rodata :{
               *(.rodata*)
         }

         .data :{
               *(.data)
         }
         /DISCARD/:{
               *(.*)
         }
}
