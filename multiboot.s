.set FLAGS, (1<<0) | (1<<1) /* 4KiB align + meminfo */
.set MAGIC,  0xe85250d6
.set ARCH, 0x0


.section .multiboot
.align 4
header_start: 
.long MAGIC
  .long ARCH
  .long (header_end - header_start)
.long 0x100000000 - (MAGIC + ARCH + (header_end - header_start))
  .short 0
  .short 0
.long 0x8
header_end: 
