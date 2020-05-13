#include "types.h"

typedef struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    // Access byte
    /*
    Has following structure
    unsigned present : 1;
    unsigned descriptor_priv_level : 2;
    unsigned descriptor_type : 1;
    unsigned segment_type : 4;
    */
    uint8_t access;
    // Granularity byte
    /*
    Has following structure
    unsigned granularity : 1;
    unsigned operand_size : 1;
    // Following two fields should always be zero
    unsigned zero : 1;
    unsigned available : 1;
    unsigned segment_length : 4;
    */
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_t;

void init_gdt();
