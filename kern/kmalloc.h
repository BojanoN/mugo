#pragma once

void  kmalloc_init(uint8_t* pool_vaddr, uint8_t* pool_phys, size_t size);
void  kfree(void* ptr);
void* kmalloc(size_t size);
