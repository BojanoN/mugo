#pragma once

#include <stdarg.h>

unsigned int vsprintf_s(char* buf, size_t size, const char* fmt, va_list args);

unsigned int strlen(const char* str);
