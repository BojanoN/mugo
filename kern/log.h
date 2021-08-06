#pragma once

#include "kprint.h"

#define log(LEVEL, format, ...) \
    kprintf("[" #LEVEL ":%s:%d] %s\n", __FILE__, __LINE__, format, ##__VA_ARGS__)
