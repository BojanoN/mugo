#pragma once

#include <types/base.h>

#define halt()       asm("hlt\t\n")
#define arch_pause() asm("pause\t\n")

void outb(uint16_t port, uint8_t val);
void outw(uint16_t port, uint16_t val);
void outl(uint16_t port, uint32_t val);

uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

uint64_t msr_read(reg_t reg);
void     msr_write(reg_t reg, uint64_t val);
