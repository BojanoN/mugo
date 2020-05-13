#include "include/interrupt.h"

irq_callback callback[NO_INTERRUPTS];

void interrupt_handler(irq_context_t context)
{
    uint32_t interrupt_num = context.int_no;
    if (interrupt_num < NO_INTERRUPTS && callback[interrupt_num]) {
        callback[interrupt_num](interrupt_num);
    }
}