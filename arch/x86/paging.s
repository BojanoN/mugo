global arch_enable_paging
global arch_load_pagetable

section .text
arch_enable_paging:
  enter
  push ecx
	movl ecx, cr0
	orl ecx, 0x80010000
	movl cr0, ecx
  pop ecx
  leave
  ret

arch_load_pagetable:
  enter
  mov cr3, [ebp+8]
  leave
  ret
