#pragma once

#include <types/base.h>

#define halt()       asm("hlt\t\n")
#define arch_pause() asm("pause\t\n")

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t val)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t val)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile("inl %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

static inline void msr_write(reg_t reg, uint64_t val)
{
    asm volatile("wrmsr"
                 :
                 : "A"(val), "c"(reg));
}

static inline uint64_t msr_read(reg_t reg)
{
    uint64_t val = 0;
    asm volatile("rdmsr"
                 :
                 : "A"(val), "c"(reg));
    return val;
}

static inline uint64_t native_cr3_read(void)
{
    unsigned long val;
    asm volatile("mov %%cr3,%0\n\t"
                 : "=r"(val)
                 :);
    return val;
}

static inline void native_cr3_write(uint32_t val)
{
    asm volatile("mov %0,%%cr3"
                 :
                 : "r"(val)
                 : "memory");
}

static inline vaddr_t native_cr2_read(void)
{
    vaddr_t val;

    asm volatile("mov %%cr2, %0"
                 : "=r"(val));

    return val;
}

void native_update_tss(vaddr_t esp0);
