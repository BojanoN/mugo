#pragma once
#include <types/base.h>

void console_clear_screen();
void console_scroll();
void console_init();
void console_move_cursor(uint16_t x, uint16_t y);
void console_set_color(uint8_t forecolor, uint8_t backcolor);
void console_write_string(const char* str);
