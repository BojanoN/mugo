#include "include/arch_panic.h"
#include <types/base.h>

#define MAX_STACK_TRACE_SIZE 32

extern uint32_t arch_stack_walk(uint32_t* arr, uint32_t maxcnt);

static const char* fmt_string                       = "cr0: 0x%08x\ncr2: 0x%08x\ncr3: 0x%08x\ncr4: 0x%08x\n";
static uint32_t    saved_eips[MAX_STACK_TRACE_SIZE] = { 0 };

void stack_trace(void)
{
    uint32_t no_callers = arch_stack_walk(saved_eips, MAX_STACK_TRACE_SIZE);

    kprintf("Stack trace:\n");
    for (uint32_t i = no_callers; i > 0; i--) {
        kprintf("0x%08x\n", saved_eips[i - 1]);
    }
}

void arch_panic_hook(void)
{
    uint32_t cr0_val = 0;
    uint32_t cr2_val = 0;
    uint32_t cr3_val = 0;
    uint32_t cr4_val = 0;

    asm("\t movl %%cr0,%%eax\n\t"
        "\t movl %%eax, %0"
        : "=r"(cr0_val));

    asm("\t movl %%cr2,%%eax\n\t"
        "\t movl %%eax, %0"
        : "=r"(cr2_val));

    asm("\t movl %%cr3,%%eax\n\t"
        "\t movl %%eax, %0"
        : "=r"(cr3_val));

    asm("\t movl %%cr4,%%eax\n\t"
        "\t movl %%eax, %0"
        : "=r"(cr4_val));

    kprintf(fmt_string, cr0_val, cr2_val, cr3_val, cr4_val);

    stack_trace();
}
