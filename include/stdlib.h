#pragma once

#include <stdarg.h>
#include <types/base.h>

unsigned int vsprintf_s(char* buf, size_t size, const char* fmt, va_list args);

unsigned int strlen(const char* str);

void* memset(void* dst, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
