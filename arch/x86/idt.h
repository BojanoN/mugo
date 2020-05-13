#pragma once
#include "types.h"

typedef struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

typedef struct idt {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_t;

void init_idt(void);
