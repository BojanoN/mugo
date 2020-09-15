#pragma once

#include <types.h>

void     PIC_sendEOI(uint8_t irqnum);
void     PIC_mask_irq(uint8_t irqnum);
void     PIC_unmask_irq(uint8_t irqnum);
void     PIC_init(void);
uint32_t PIC_check_spurious(void);

typedef struct {
    uint64_t no_interrupts;
    uint64_t no_spurious_interrupts;
} pic_stats_t;

// TODO: add interrupt stats tracking
