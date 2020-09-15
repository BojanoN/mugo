#include "include/interrupt.h"
#include "drivers/pic.h"
#include <kern/kprint.h>

#define NO_EXCEPTIONS 32

irq_callback callback[NO_INTERRUPTS] = { 0 };

static char* exception_string[NO_EXCEPTIONS] = {
    "Divide by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    // Legacy, used for external FPU
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "Floating point exception",
    "Alignment check",
    "Machine check",
    "SIMD floating point exception",
    "Virtualization exception",
    "Reserved",
    "Security exception",
    "Reserved",
    "Triple fault",
    // Legacy, used for external FPU
    "FPU error interrupt"
};

void exception_handler(irq_context_t context)
{

    if (PIC_check_spurious()) {
        return;
    }

    uint32_t interrupt_num = context.int_no;

    PIC_sendEOI(interrupt_num);

    kprintf("Received exception no. %d: %s \n", interrupt_num, exception_string[interrupt_num]);
    return;
}

void interrupt_handler(irq_context_t context)
{
    if (PIC_check_spurious()) {
        return;
    }

    uint32_t interrupt_num = context.int_no;

    PIC_sendEOI(interrupt_num);

    if (interrupt_num < NO_INTERRUPTS && callback[interrupt_num]) {
        callback[interrupt_num](interrupt_num);
    }
    kprintf("Received interrupt %d \n", interrupt_num);
    return;
}
