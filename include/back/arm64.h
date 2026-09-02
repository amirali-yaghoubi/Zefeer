#pragma once
#include "middle/ir.h"


typedef struct {
    FILE* out;
} ARM64Context;

void arm64_init(ARM64Context* ctx, IRInstruction* irc, const char* file_name);

