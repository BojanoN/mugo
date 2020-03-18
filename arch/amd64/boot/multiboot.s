mb2_magic: equ 0xe85250d6
mb2_size: equ mb_end - mb_start

section .multiboot
mb_start:
    dd mb2_magic              
    dd 0 
    dd mb2_size
    dd 0x100000000 - (mb2_magic + 0 + mb2_size)

    ; insert optional multiboot tags here

    ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
mb_end:
