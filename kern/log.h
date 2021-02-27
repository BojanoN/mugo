#pragma once

#include "kprint.h"

#define log(LEVEL, format, ...) \
    kprintf("[" #LEVEL ":%s:%d] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
