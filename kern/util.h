#pragma once
#include <arch/util.h>
#include <sys/cdefs.h>

#define pause() arch_pause()

NORETURN void panic(const char* msg);
