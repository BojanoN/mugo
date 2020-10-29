#include <console.h>
#include <string.h>
#include <types.h>

typedef struct __attribute__((packed)) {
    uint8_t character;
    uint8_t color_attrib;
} vga_char_t;

static const uint16_t VGA_WIDTH  = 80;
static const uint16_t VGA_HEIGHT = 25;
static const uint16_t VGA_TOTAL  = VGA_HEIGHT * VGA_WIDTH;

static vga_char_t* term_buff;
static uint8_t*    vga_cmd_port;

static uint16_t x = 0;
static uint16_t y = 0;

static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;

// TODO Refactor this
static uint8_t color_attrib = (0x0 << 4) | 0xF;

void console_init()
{
    term_buff    = (vga_char_t*)0xC03FF000;
    vga_cmd_port = (uint8_t*)0xC03FF3D4;

    console_clear_screen();
    //console_move_cursor(0, 0);
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

    vga_char_t clr = { ' ', 0 };

    for (unsigned int i = 0; i < VGA_TOTAL; i++) {
        term_buff[i] = clr;
    }
}

void console_set_color(uint8_t forecolor, uint8_t backcolor)
{
    color_attrib = (backcolor << 4) | forecolor;
}

void console_scroll()
{
    vga_char_t clr = { ' ', 0 };

    for (unsigned int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
        term_buff[i] = term_buff[i + VGA_WIDTH];
    }

    for (unsigned int j = VGA_WIDTH * (VGA_HEIGHT - 1); j < VGA_TOTAL; j++) {
        term_buff[j] = clr;
    }
}

void console_write_string(const char* str)
{
    unsigned int n = strlen(str);

    vga_char_t chr;

    chr.color_attrib = color_attrib;

    for (unsigned int i = 0; i < n; i++) {

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

        chr.character = str[i];

        term_buff[(y * VGA_WIDTH) + x] = chr;

        if (x >= VGA_WIDTH) {
            x = 0;
            y++;
        }

        if (y >= VGA_HEIGHT) {
            console_scroll();
        }

        x++;
    }

    //  console_move_cursor(x, y);
}
