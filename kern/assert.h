#pragma once

#include "log.h"
#include "util.h"

#ifdef DEBUG

#define ASSERT(expr)         \
    do                       \
        if (!(expr)) {       \
            log(ASSERT, ""); \
            halt();          \
        }                    \
    while (0)

#define ASSERT_MSG(expr, msg) \
    do                        \
        if (!(expr)) {        \
            log(ASSERT, msg); \
            halt();           \
        }                     \
    while (0)

#else

#define ASSERT(expr)
#define ASSERT_MSG(expr, msg)

#endif
