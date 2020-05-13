#pragma once
#include "types.h"

#define NO_INTERRUPTS 256
typedef void (*irq_callback)(uint32_t);

typedef struct irq_context {
    uint32_t ds; // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t int_no, err_code; // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} irq_context_t;
