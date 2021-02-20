#pragma once
#include <arch/util.h>

#define pause() arch_pause()

void panic(const char* msg);
