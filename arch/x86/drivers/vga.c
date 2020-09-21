#include <console.h>
#include <string.h>
#include <types.h>

static const uint16_t VGA_WIDTH  = 80 * 2;
static const uint16_t VGA_HEIGHT = 25;

static uint8_t* term_buff    = (uint8_t*)0xB8000;
static uint8_t* vga_cmd_port = (uint8_t*)0x3D4;

static uint32_t x = 0;
static uint32_t y = 0;

static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

// TODO Refactor this
static uint8_t color_attrib = (0x0 << 4) | 0xF;

void console_init()
{
    console_clear_screen();
    console_move_cursor(0, 0);
}

void console_move_cursor(uint16_t x, uint16_t y)
{
    cursor_x = x;
    cursor_y = y;

    uint16_t new_location = x * 80 + y;

    vga_cmd_port[0] = 14;
    vga_cmd_port[1] = new_location >> 8;
    vga_cmd_port[0] = 15;
    vga_cmd_port[1] = new_location & 0xFF;
}

void console_clear_screen()
{
    char clr = ' ';

    for (unsigned int i = 0; i < VGA_HEIGHT; i++) {
        for (unsigned int j = 0; j < VGA_WIDTH; j += 2) {
            term_buff[(i * VGA_WIDTH) + j]     = clr;
            term_buff[(i * VGA_WIDTH) + j + 1] = 0;
        }
    }
}

void console_set_color(uint8_t forecolor, uint8_t backcolor)
{
    color_attrib = (backcolor << 4) | forecolor;
}

void console_scroll()
{
    for (unsigned int i = 0; (i + 1) < VGA_HEIGHT; i++) {
        for (unsigned int j = 0; j < VGA_WIDTH; j++) {
            term_buff[(i * VGA_WIDTH) + j] = term_buff[((i + 1) * VGA_WIDTH) + j];
        }
    }

    for (unsigned int j = 0; j < VGA_WIDTH; j += 2) {
        term_buff[(24 * VGA_WIDTH) + j]     = ' ';
        term_buff[(24 * VGA_WIDTH) + j + 1] = 0;
    }
}

void console_write_string(const char* str)
{
    unsigned int n = strlen(str);

    for (unsigned int i = 0; i < n; i++, x += 2) {

        if (str[i] == '\n') {
            y++;
            i++;
            x = 0;
            continue;
        }
        if (str[i] == '\r') {
            x = 0;
            i++;
            continue;
        }

        term_buff[(y * VGA_WIDTH) + x]     = str[i];
        term_buff[(y * VGA_WIDTH) + x + 1] = color_attrib;

        if (x >= VGA_WIDTH) {
            x = 0;
            y++;
        }

        if (y >= VGA_HEIGHT) {
            console_scroll();
        }
    }

    console_move_cursor(x, y);
}
