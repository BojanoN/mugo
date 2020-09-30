#pragma once

#include <arch_paging.h>

typedef arch_page_t page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t;

typedef struct page_directory {
    page_table_t* tables[1024];
    unsigned int  tablesPhysical[1024];
    unsigned int  physicalAddr;
} page_directory_t;
