CMAKE_MINIMUM_REQUIRED(VERSION 3.15.0)


project(praxOS C)

include(Macros.cmake)

# TODO determine via variable
set(TARGET_ARCH i686)
set(OUTPUT_FILE os.elf)

set(CMAKE_C_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/toolchain/llvm-project/build/bin/clang")
set(CMAKE_C_CLANG_TIDY "clang-tidy10;-checks=*")

message(STATUS "Build target: " ${TARGET_ARCH})

add_executable(${OUTPUT_FILE})

target_include_directories(${OUTPUT_FILE} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/kern
  ${CMAKE_CURRENT_SOURCE_DIR}/
  )

add_subdirectory(./arch/${TARGET_ARCH})
add_subdirectory(./serv/mman)

set(kernel_sources
  ./kern/kmalloc.c
  ./kern/main.c
  ./kern/time.c
  ./kern/util.c
  ./kern/sys.c
  ./kern/proc.c
  ./kern/panic.c
  ./kern/kmem.c
  ./kern/kprint.c
  )
list(TRANSFORM kernel_sources PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)


set(lib_sources
  ./lib/stdlib/stdlib.c
  ./lib/libelf/elf.c
  )

list(TRANSFORM lib_sources PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

target_sources(${OUTPUT_FILE}
  PRIVATE
  ${kernel_sources}
  ${lib_sources}
  )


#add_subdirectory(./kern)
#add_subdirectory(./lin)


