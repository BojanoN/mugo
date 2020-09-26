extern arch_entrypoint, stack

global arch_start

bits 32

vga_buf equ 0xb8000
color equ 0x0f

boot_msg db	'Jumping to kernel_start...', 0

print_string:
    pusha
    mov ebx, vga_buf

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

  push ebx
  push eax

  call arch_entrypoint
  jmp $
