CMAKE_MINIMUM_REQUIRED(VERSION 3.15.0)


project(Mugo C)
cmake_policy(SET CMP0115 OLD)

#include(Macros.cmake)

# TODO determine via variable
set(TARGET_ARCH i686)
set(OUTPUT_FILE os.elf)

set(TOOLCHAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/bin)

set(CMAKE_C_COMPILER "${TOOLCHAIN_DIR}/clang")
#set(CMAKE_C_CLANG_TIDY "${TOOLCHAIN_DIR}/clang-tidy;-checks=*")

message(STATUS "Build target: " ${TARGET_ARCH})

add_executable(${OUTPUT_FILE})

target_include_directories(${OUTPUT_FILE} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/kern
  ${CMAKE_CURRENT_SOURCE_DIR}/
  )

add_subdirectory(./arch/${TARGET_ARCH})
add_subdirectory(./sys/arch/${TARGET_ARCH})
add_subdirectory(./serv/mman)

foreach(ASM_SRC ${asm_preprocess_sources})
  string(REPLACE ".S" "" ASM_FILE ${ASM_SRC})
  message(STATUS ${ASM_FILE})
  add_custom_command(
    OUTPUT  ${ASM_FILE}
    COMMAND "${CMAKE_C_COMPILER}" -E ${ASM_SRC} -o ${ASM_FILE}
    MAIN_DEPENDENCY ${ASM_SRC}
    COMMENT "Preprocessing ${ASM_SRC}"
    VERBATIM
    )
endforeach()


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
  ./kern/sched.c
  ./kern/sched_rr.c
  ./kern/kcall.c
  ./kern/crt.c
  )
list(TRANSFORM kernel_sources PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)


set(klib_sources
  ./lib/stdlib/stdlib.c
  ./lib/libelf/elf.c
  )
list(TRANSFORM klib_sources PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)



target_compile_definitions(${OUTPUT_FILE} PRIVATE __MUGO_KERNEL__)
target_sources(${OUTPUT_FILE}
  PRIVATE
  ${kernel_sources}
  ${klib_sources}
  )




