#pragma once
#include <types.h>

typedef struct console_color {
    uint8_t forecolor;
    uint8_t backcolor;
} __attribute__((packed)) console_color_t;

void console_clear_screen();
void console_set_color(uint8_t forecolor, uint8_t backcolor);
void console_write_string(const char* str);
