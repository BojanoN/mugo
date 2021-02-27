#pragma once

#include <arch/interrupt.h>
#include <types/base.h>

#define KMAP_PROT_NONE  0x0 // page is inaccessible (guard pages)
#define KMAP_PROT_READ  0x1 // page is readable
#define KMAP_PROT_WRITE 0x2 // page is writeable
#define KMAP_PROT_USER  0x4 // page is user accessible
#define KMAP_PROT_EXEC  0x8 // page is executable

void map_kernel_memory(void);
void page_fault_handler(irq_context_t registers);
void kmem_init(void);
