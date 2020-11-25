global arch_stack_walk

bits 32
section .text
arch_stack_walk:

  push ebp
  mov ebp, esp

 	push edi
  push ebx

  xor eax, eax
  mov edi, [ebp + 8] 	;; array pointer
	mov ecx, [ebp + 12]	  ;; array length
  mov ebx, [ebp] 	  ;; caller ebp


.descent:

  test ebx, ebx
  jz .restore

  mov edx, [ebx + 4]
  mov ebx, [ebx]

  mov [edi], edx
  add edi, 4
  inc eax

  loop .descent

.restore:
  pop ebx
  pop edi

  leave
  ret
