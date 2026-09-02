#include "back/arm64.h"


void arm64_init(ARM64Context* ctx, IRInstruction* irc, const char* file_name)
{
    ctx->out = fopen(file_name, "wb");
}