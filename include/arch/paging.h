#pragma once

#include <arch_paging.h>

typedef arch_pt_entry_t pt_entry_t;
typedef arch_pd_entry_t pd_entry_t;

typedef struct page_table {
    pt_entry_t entries[1024];
} page_table_t;

typedef struct page_directory {
    pd_entry_t entries[1024];
} page_directory_t;

#define PAGE_SIZE ARCH_PAGE_SIZE
#define PAGE_MASK ARCH_PAGE_MASK
