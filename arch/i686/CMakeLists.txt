
message(STATUS "Setting target flags...")

enable_language(ASM_NASM)



set(CMAKE_C_FLAGS "-Wall -Werror -Wextra -Wno-unused-parameter  -target i686-pc-mugo-elf -march=i686 -ffreestanding -nostdlib -fno-builtin  -fno-pie -fno-pic -mno-mmx -mno-sse -mno-sse2" PARENT_SCOPE)
set(CMAKE_ASM_NASM_FLAGS "-felf32 -w+error=all" PARENT_SCOPE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS "s" PARENT_SCOPE)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>" PARENT_SCOPE)

target_include_directories(${OUTPUT_FILE} PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_options(${OUTPUT_FILE} PUBLIC --prefix ${TOOLCHAIN_DIR} -T ${CMAKE_CURRENT_SOURCE_DIR}/link.ld  -fuse-ld=lld -v)

set(c_sources
  panic.c
  interrupt.c
  boot/gdt.c
  util.c
  config
  idt.c
  drivers/vga.c
  drivers/pic.c
  drivers/timer.c
  early.c
  thread.c
  init.c
  )
list(TRANSFORM sources PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

set(asm_sources
  sysenter.asm
  boot/multiboot.asm
  boot/load_tables.asm
  boot/boot.asm
  boot/interrupt.asm
  thread_asm.asm
  trace.asm
  )
list(TRANSFORM asm_sources PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

set_source_files_properties(${asm_sources} PROPERTIES LANGUAGE ASM_NASM)

target_sources(${OUTPUT_FILE}
  PRIVATE
  ${c_sources}
  ${asm_sources}
 )


