#pragma once

#include <types.h>

#define PAGEFRAME_FLAG_FREE        1
#define PAGEFRAME_FLAG_UNAVAILABLE 2
// 6 bits are reserved for future use

struct page_frame {
    unsigned int       virtual_addr;
    struct page_frame* next;
    uint32_t           flags;
    uint32_t           cksum;
} __attribute__((packed));

void pageframe_alloc_init(struct multiboot_info* multiboot_info_ptr);

unsigned int alloc_frame(void);
void         free_frame(unsigned int addr);
