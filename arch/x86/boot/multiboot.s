mb2_magic: equ 0xe85250d6
mb2_size: equ mb_end - mb_start
	MODULEALIGN equ  1<<0                   ; align loaded modules on page boundaries
	MEMINFO     equ  1<<1                   ; provide memory map (BIOS e820)
	FLAGS       equ  MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
	MAGIC       equ  0x1BADB002             ; magic number lets bootloader find the header
	CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required


section .multiboot
mb_start:
	   dd MAGIC
	   dd FLAGS
	   dd CHECKSUM
    dd mb2_magic
    dd 0
    dd 0x1c
    dd 0x100000000 - (mb2_magic + 0 + 0x1c)

    ; insert optional multiboot tags here

    ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
mb_end:
