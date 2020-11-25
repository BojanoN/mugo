
extern arch_entrypoint, stack, early_stack, early_main
extern __kernel_start, __kernel_end, K_HIGH_VMA

bits 32

vga_buf_virt equ 	0xC03FF000
color equ 0x0f

section .boot.data
multiboot_magic:
	dd 0
multiboot_info_ptr:
	dd 0

global page_directory
global kernel_pt
global metadata_pt

;; Dont zero init, just allocate space
section .boot.bss, nobits
align 4096
page_directory:
  resb 4096
kernel_pt:
  resb 4096
metadata_pt:
  resb 4096

section  .boot.text

global _start

_start:

  mov esp, early_stack + 0x1000

  mov dword [multiboot_magic], eax
  mov dword [multiboot_info_ptr], ebx


  call early_main

  ;; Point cr3 to page directory
	mov ecx, page_directory
  sub ecx, K_HIGH_VMA
  mov cr3, ecx

  ;; Enable paging
	mov ecx, cr0
	or ecx, 0x80010000
	mov cr0, ecx


  ;; Jump to higher half
  lea eax, arch_start
  jmp eax



section .rodata
boot_msg db	'Jumping to kernel_start...', 0


section .text
print_string:
    pusha
    mov ebx, vga_buf_virt

.loop:
    mov al, [edx]
    mov ah, color

    cmp al, 0
    je .ret

    mov [ebx], ax
    add edx, 1
    add ebx, 2

    jmp .loop

.ret:
    popa
    ret

error:
  hlt

arch_start:

  mov esp, stack + 0x10000

	push	0
	popf

  mov edx, boot_msg
  call print_string

	push dword [multiboot_info_ptr]
  push dword [multiboot_magic]

  xor ebp, ebp 	  ;; Stack trace sentry
  call arch_entrypoint
  jmp $
