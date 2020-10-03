global init_paging
extern __kernel_start, __kernel_end, K_HIGH_VMA


  ;; Dont zero init, just allocate space
section .boot_page_tables, 	nobits

align 4096
page_directory:
  resb 4096
kernel_pt:
  resb 4096
boot_pt:
  resb 4096

section  .boot.text

_start: 

.fill_pd:
	 loop .fill_pd


