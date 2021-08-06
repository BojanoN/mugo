#pragma once

#include <arch/info.h>
#include <arch/interrupt.h>
#include <types/base.h>

#define KMAP_PROT_NONE  0x0 // page is inaccessible (guard pages)
#define KMAP_PROT_READ  0x1 // page is readable
#define KMAP_PROT_WRITE 0x2 // page is writeable
#define KMAP_PROT_USER  0x4 // page is user accessible
#define KMAP_PROT_EXEC  0x8 // page is executable

void kmem_map_kernel_memory(kernel_info_t* info);
void page_fault_handler(irq_context_t registers);

void kmem_init(kernel_info_t* info);
void kmem_init_cleanup(void);

typedef struct exec_info exec_info_t;

void    bootstrap_identity_map_init_mem(kernel_info_t* info);
paddr_t bootstrap_create_page_dir(void);
int     bootstrap_mmap(exec_info_t* info, paddr_t paddr, vaddr_t vaddr, size_t size, unsigned int flags);
int     bootstrap_mmap_elf(exec_info_t* info, paddr_t paddr, vaddr_t vaddr, size_t size, unsigned int flags);
