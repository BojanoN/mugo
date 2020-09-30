global init_paging
  ;; Dont zero init, just allocate space
section .boot_page_tables, 	nobits

extern __kernel_start, __kernel_end

UPPER_HALF: equ 0xC0000000

align 4096
page_directory:
   resb 4096
page_table:
   resb 4096

section  .text
  %macro k_phys 1
     mov %1, (__kernel_start - UPPER_HALF)
  %endmacro

init_paging:

  k_phys eax


.fill_pd:
	 loop .fill_pd


