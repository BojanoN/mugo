#include <console.h>
#include <string.h>
#include <types.h>

static const uint16_t VGA_WIDTH  = 80 * 2;
static const uint16_t VGA_HEIGHT = 25 * 2;

static unsigned char* term_buff = (unsigned char*)0xB8000;

static uint32_t x = 0;
static uint32_t y = 0;

static console_color_t color = { .forecolor = 0xF, .backcolor = 0x0 };

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
    color.forecolor = forecolor;
    color.backcolor = backcolor;
}

void console_write_number(uint32_t num)
{
}

void console_write_string(const char* str)
{
    unsigned char attrib = (color.backcolor << 4) | (color.forecolor);

    unsigned int n = strlen(str);

    for (unsigned int i = 0; i < n; i++, x += 2) {
        if (str[i] != '\n') {
            term_buff[(y * VGA_WIDTH) + x]     = str[i];
            term_buff[(y * VGA_WIDTH) + x + 1] = attrib;
        } else {
            y++;
            x = 0;
        }
        if (x >= VGA_WIDTH) {
            x = 0;
            y++;
        }
    }
}
