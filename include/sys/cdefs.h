#pragma once

#define NORETURN    __attribute__((noreturn))
#define UNREACHABLE __builtin_unreachable()

#define unlikely(expr) __builtin_expect(!!(expr), 0)
#define likely(expr)   __builtin_expect(!!(expr), 1)
