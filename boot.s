extern kernel_start, stack, long_mode_init

global arch_start

;; Paging tables
section .bss
	align 4096
	pml4_table:
	    resb 4096
	pdp_table:
	    resb 4096
	pd_table:
	    resb 4096

section .rodata
gdt64:
  	  dq 0
	  .code: equ $ - gdt64
	    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
	  .addr:
	    dw $ - gdt64 - 1
	dq gdt64

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

  ;; Paging setup
  mov eax, pdp_table
  ;; https://wiki.osdev.org/Paging
  or eax, 0b11
  mov [pml4_table], eax

	mov eax, pd_table
  or eax, 0b11
  mov [pdp_table], eax

  ;; PD table fill
  mov ecx, 256
  mov ebx, 0

  .fill_pd
	mov eax, 0x200000
  mul ebx
	or eax, 0b10000011
  mov [pd_table + ebx*8], eax
	inc ebx
  loop .fill_pd

  ;; Enable paging
  mov eax, pml4_table
  mov cr3, eax


  ;; Enable PAE
  mov eax, cr4
  or eax, 1<<5
  mov cr4, eax

	;; Flip longmode bit
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1<<8
  wrmsr

	mov eax, cr0
  or eax, 1<<31
  mov cr0, eax


	;; GDT
  lgdt [gdt64.addr]

  jmp gdt64.code:long_mode_init

  cli
  hlt


