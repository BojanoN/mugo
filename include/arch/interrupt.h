#pragma once

#include <arch_interrupt.h>

typedef struct irq_context irq_context_t;

typedef void (*irq_callback)(irq_context_t);
void register_interrupt_callback(uint8_t num, irq_callback cb);
