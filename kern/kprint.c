#include "kprint.h"
#include <arch/interrupt.h>
#include <stdlib.h>

unsigned int kprintf(char* fmt, ...)
{
    char         buff[KPRINT_BUFF_LEN];
    unsigned int print_size = vsprintf(buff, KPRINT_BUFF_LEN, &fmt);

    console_write_string(buff);

    return print_size;
};
