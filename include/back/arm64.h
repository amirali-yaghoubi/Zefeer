#pragma once
#include "middle/ir.h"


typedef struct {
    FILE* out;
    IRContext* irc;
    int num_vars;          // total_slots from semantic
    int max_temp;          // highest temp used
    int total_slots;       // num_vars + max_temp + 1
    int frame_size;
} ARM64Context;

void arm64_init(ARM64Context* ctx, IRContext* irc, const char* file_name);

