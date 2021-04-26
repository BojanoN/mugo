set(MMAN_ELF_MODULE mman.elf PARENT_SCOPE)
set(MMAN_ELF_MODULE mman.elf )


list(TRANSFORM sources PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)


add_executable(${MMAN_ELF_MODULE})

target_link_options(${MMAN_ELF_MODULE} PUBLIC -T ${CMAKE_CURRENT_SOURCE_DIR}/link.ld -fuse-ld=lld)


set(c_sources
  ./main.c
  )

target_sources(${MMAN_ELF_MODULE}
  PRIVATE
  ${c_sources}
  )
