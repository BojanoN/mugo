#include "kprint.h"
#include <arch/interrupt.h>
#include <stdarg.h>
#include <stdlib.h>

unsigned int kprintf(const char* fmt, ...)
{
    char buff[KPRINT_BUFF_LEN];

    va_list args;
    va_start(args, fmt);

    unsigned int print_size = vsprintf_s(buff, KPRINT_BUFF_LEN, fmt, args);

    va_end(args);

    console_write_string(buff);

    return print_size;
};
