	MAGIC       equ  0xe85250d6

  MULTIBOOT_HEADER_TAG_END  equ 0
  MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST  equ 1
  MULTIBOOT_HEADER_TAG_MODULE_ALIGN  equ 6

  MULTIBOOT_TAG_TYPE_MODULE equ 3
	MULTIBOOT_TAG_TYPE_BASIC_MEMINFO equ 4
  MULTIBOOT_TAG_TYPE_MMAP equ 6
  MULTIBOOT_TAG_TYPE_ELF_SECTIONS equ 9

section .multiboot
start:
	dd MAGIC
	dd 0
	dd end - start


     ;; checksum
  dd 0x100000000 - (MAGIC + 0 + (end - start))


module_align_tag:
  dw MULTIBOOT_HEADER_TAG_MODULE_ALIGN
  dw 0
  dd module_align_tag_end - module_align_tag
module_align_tag_end:

info_request_tag:
	dw MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST 
	dw 0
	dd info_request_tag_end - info_request_tag
  ;; Requested tags
  dd MULTIBOOT_TAG_TYPE_BASIC_MEMINFO
  dd MULTIBOOT_TAG_TYPE_MMAP
  dd MULTIBOOT_TAG_TYPE_ELF_SECTIONS
  dd MULTIBOOT_TAG_TYPE_MODULE
info_request_tag_end:


terminating_tag:
  dw MULTIBOOT_HEADER_TAG_END
  dw 0
  dd 8
end:
