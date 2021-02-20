#pragma once

#include <arch/interrupt.h>
#include <types/base.h>

#define PAGEFRAME_FLAG_FREE        1
#define PAGEFRAME_FLAG_UNAVAILABLE 2
// 6 bits are reserved for future use

struct page_frame {
    unsigned int       virtual_addr;
    struct page_frame* next;
    uint32_t           flags;
} __attribute__((packed));

void init_free_memory(struct multiboot_info* multiboot_info);
void map_kernel_memory(void);
void page_fault_handler(irq_context_t registers);
