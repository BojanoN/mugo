#pragma once

#include <meminfo.h>
#include <types/base.h>

int kcall_debug(const char* string);
int kcall_meminfo(struct sys_meminfo* meminfo);
