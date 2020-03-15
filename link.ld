ENTRY(arch_start)

SECTIONS {
         . = 1M;

         .boot :{
               *(.multiboot)
               *boot.o ( .text* )
         }

         .text :{
               *(.text)
         }
}
