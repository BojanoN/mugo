extern kernel_start, stack

global arch_start

bits 32

vga_buf equ 0xb8000
color equ 0x0f

multiboot_err db	'Multiboot structure not loaded!'
boot_msg db	'Jumping to kernel_start...'


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

.no_multiboot:
  mov edx, multiboot_err
  call print_string
  jmp error


arch_start:
  mov esp, stack

  ;; Check if multiboot was loaded
	cmp eax, 0x36d76289
	jne error.no_multiboot

  mov edx, boot_msg
  call print_string

  call kernel_start
  jmp $
