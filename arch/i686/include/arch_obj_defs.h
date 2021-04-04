#pragma once

#include <types/base.h>

typedef struct {
    reg_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
} arch_gp_regs;

typedef struct {
    arch_gp_regs gp_regs;
    uint16_t     gs, fs, es, ds;
    reg_t        eip, cs, eflags, useresp, ss;
} arch_regs;

typedef struct {
    arch_gp_regs gp_regs_state;
    uint32_t     err;

    reg_t eip;
    reg_t cs;
    reg_t eflags;
    reg_t esp;
    reg_t ss;
} arch_thread_context_t;

// clang-format off
struct tss{
        unsigned  : 32;                   // 0x0

        uint32_t  sp0;                    // 0x4
        uint16_t  ss0;  unsigned : 16;      // 0x8
        uint32_t  sp1;                    // 0xc
        uint16_t  ss1;  unsigned : 16;      // 0x10
        uint32_t  sp2;                    // 0x14
        uint16_t  ss2;  unsigned : 16;      // 0x18
        uint32_t  cr3;                    // 0x1c
        uint32_t  eip;                    // 0x20
        uint32_t  eflags;                 // 0x24
        uint32_t  eax;                    // 0x28
        uint32_t  ecx;                    // 0x2c
        uint32_t  edx;                    // 0x30
        uint32_t  ebx;                    // 0x34
        uint32_t  esp;                    // 0x38
        uint32_t  ebp;                    // 0x3c
        uint32_t  esi;                    // 0x40
        uint32_t  edi;                    // 0x44
        uint16_t  es;   unsigned : 16;      // 0x48
        uint16_t  cs;   unsigned : 16;      // 0x4c
        uint16_t  ss;   unsigned : 16;      // 0x50
        uint16_t  ds;   unsigned : 16;      // 0x54
        uint16_t  fs;   unsigned : 16;      // 0x58
        uint16_t  gs;   unsigned : 16;      
        uint16_t  ldtr; unsigned : 16;
  unsigned : 32;
};
// clang-format on
