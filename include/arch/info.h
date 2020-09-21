#pragma once
#include "interrupt.h"

typedef struct {
    const char* name;

    void (*init)(void);
    void (*halt)(void);

} arch_info_t;
