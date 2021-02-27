#pragma once

#include "log.h"
#include "util.h"

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
