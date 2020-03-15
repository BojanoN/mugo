extern kernel_start, stack

global arch_start

section .text
  bits 32


vga_buf equ 0xb8000
color equ 0x0f ; the color byte for each character

multiboot_err db	'Multiboot structure not loaded!'
cpuid_err db	'CPUID not supported!'
longmode_err db	'Longmode not supported!'


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

.no_cpuid:
	mov edx, cpuid_err
	call print_string
	jmp error

.no_longmode
	mov edx, longmode_err
	call print_string
	jmp error

arch_start:
  mov esp, stack

  ;; Check if multiboot was loaded
	cmp eax, 0x36d76289
	jne error.no_multiboot

  ;; Check if CPUID is supported
	pushfd
  pop eax
  mov ebx, eax

  xor eax, 1 << 21
  push eax
  popfd

  pushfd
  pop eax

  cmp eax, ecx
  je error.no_cpuid

  ;; Check if long mode is supported
	mov eax, 0x80000000
  cpuid
  cmp eax, 0x80000001
  jb error.no_longmode

	mov eax, 0x80000001
  cpuid
	test edx, 1 << 29
	jb error.no_longmode

  push	0
	popf

 	mov dword [0xb8000], 0x2f4b2f4f
  hlt

	call kernel_start

  cli
  hlt


