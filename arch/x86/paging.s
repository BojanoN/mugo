global arch_enable_paging
global arch_load_pagetable

section .text
arch_enable_paging:
  ;;  enter
  push ecx
	mov ecx, cr0
	or ecx, 0x80010000
	mov cr0, ecx
  pop ecx
  ;;  leave
  ret

arch_load_pagetable:
  ;;   enter
  ;;   mov cr3, [ebp+8]
  ;;   leave
  ret
