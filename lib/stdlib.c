#include <console.h>
#include <stdlib.h>

#define NUM_BUF_MAX 50

static void itoa(unsigned int num, char* buf, size_t size)
{
    char*        tmp = buf;
    unsigned int i;

    do {
        *tmp++ = (num % 10) + '0';
        num /= 10;
        i++;
    } while (num > 0 && i < size);
    *tmp = 0;

    char* p1 = tmp - 1;
    char* p2 = buf;

    while (p1 > p2) {
        char c = *p2;
        *p2    = *p1;
        *p1    = c;

        p1--;
        p2++;
    }
}

static char hexchars[] = "0123456789abcdef";

static void xtoa(unsigned int num, char* buf, size_t bufsize)
{

    char*        tmp = buf;
    unsigned int i;

    do {
        *tmp++ = hexchars[num % 16];
        num /= 16;
        i++;
    } while (num > 0 && i < bufsize);
    *tmp = 0;

    char* p1 = tmp - 1;
    char* p2 = buf;

    while (p1 > p2) {
        char c = *p2;
        *p2    = *p1;
        *p1    = c;

        p1--;
        p2++;
    }
}

unsigned int vsprintf_s(char* dstbuf, size_t size, const char* fmt, va_list args)
{
    char*        s;
    unsigned int written = 0;

    char num_buf[NUM_BUF_MAX];

    for (; *fmt && (written < (size - 1)); fmt++) {

        if (*fmt != '%') {
            *dstbuf++ = *fmt;
            written++;
            continue;
        }

        fmt++;

        switch (*fmt) {
        case 's': {
            int len;

            s = va_arg(args, char*);

            if (!s) {
                s   = "null";
                len = 4;
            } else {
                len = strlen(s);
            }

            if (len) {
                // Fill buffer until end
                if (len + written > size) {
                    len = (size - written);
                }

                for (int i = len; i > 0; i--) {
                    *dstbuf++ = *s++;
                }

                written += len;
            }
            break;
        }
        case 'c': {
            *dstbuf = (unsigned char)va_arg(args, int);
            dstbuf++;

            written++;
            break;
        }
        case 'd':
        case 'u': {
            int num = va_arg(args, unsigned int);

            itoa(num, num_buf, NUM_BUF_MAX);
            char* tmp = num_buf;

            while (*tmp) {
                *dstbuf++ = *tmp++;
            }

            break;
        }
        case 'x': {
            int num = va_arg(args, unsigned int);

            xtoa(num, num_buf, NUM_BUF_MAX);
            char* tmp = num_buf;

            while (*tmp) {
                *dstbuf++ = *tmp++;
            }

            break;
        }

        default:
            if (*fmt != '%') {
                *dstbuf++ = '%';
                written++;
            }
            break;
        }
    }

    //flush:
    *dstbuf++ = '\0';

    return written;
}

unsigned int strlen(const char* str)
{
    unsigned int len = 0;

    while (str[len] != 0) {
        len++;
    }

    return len;
}
