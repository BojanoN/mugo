#pragma once

#include "kobjs.h"
#define NO_KCALLS         1
#define NO_KCALL_ARGS_MAX 5

typedef struct {
    size_t kcall_number;
    word_t args[NO_KCALL_ARGS_MAX];
} kcall_frame_t;

typedef int (*kcall_t)(tcb_t*, void*);

int kcall_handler(kcall_frame_t* frame);
