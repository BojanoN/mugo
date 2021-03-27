
  bits 32

  global native_sysexit_to_user, native_iret_to_user

native_iret_to_user:
 	push    ebp
	mov     ebp, esp

  ;; Entrypoint EIP
	mov     ecx, [ebp+8]

	mov ax, (4 * 8) | 3 ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax ; SS is handled by iret
	 
	; set up the stack frame iret expects
	mov eax, esp
	push (4 * 8) | 3 ; data selector
	push eax ; current esp
	pushf ; eflags
	push (3 * 8) | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	push ecx ; instruction address to return to
	iret

native_sysexit_to_user:
	push    ebp
	mov     ebp, esp

  xor eax, eax
	  mov ax, 0x23
	  mov ds, ax
	  mov es, ax
	  mov fs, ax
	  mov gs, ax

	;; EFLAGS reset
	  push 0
	  popf


  ;; esp
	mov     ecx, [ebp+8]
  ;; eip
	mov     edx, [ebp+12]

	sti

    sysexit


