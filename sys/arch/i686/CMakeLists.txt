set(kcall_sources_local
  syscall.c
  kcall_stub.asm
  )
list(TRANSFORM kcall_sources_local PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

set(kcall_sources
  ${kcall_sources_local}
  PARENT_SCOPE
  )
