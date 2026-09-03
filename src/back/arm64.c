#include "back/arm64.h"


void arm64_init(ARM64Context* ctx, IRContext* irc, const char* file_name)
{
    ctx->out = fopen(file_name, "wb");
    ctx->int_size_bite = 32;
    ctx->stack_alignment = 16;
    ctx->frame_size = ((ctx->int_size_bite * ctx->irc->ctx->total_slots / 8) + ctx->stack_alignment - 1) & ~(ctx->stack_alignment - 1);
    ctx->irc = irc;
}


//====Helpers====
static void emit_prologue(ARM64Context* ctx)
{
    fprintf(ctx->out, "    str x30, [sp, #-16]!\n");
    fprintf(ctx->out, "    sub sp, sp, #%d\n", ctx->frame_size);
}


static void emit_epilogue(ARM64Context* ctx)
{
    fprintf(ctx->out, "    add sp, sp, #%d\n", ctx->frame_size);
    fprintf(ctx->out, "    ldr x30, [sp], #16\n");
    fprintf(ctx->out, "    ret\n");
}


static int get_var_offset(ARM64Context* ctx, int var_id)
{
    return var_id * 4;
}


static int get_temp_offset(ARM64Context* ctx, int temp_id)
{
    return (ctx->num_vars + temp_id) * 4;
}


static void emit_load_oprenad(ARM64Context* ctx, IROperand op)
{
    switch(op.type)
    {
        case IR_OPERAND_CONST:
        fprintf(ctx->out, "    mov w0, #%d\n", op.value);

        case IR_OPERAND_VAR:
        fprintf(ctx->out, "    ldr w0, [sp, #%d]\n", get_var_offset(ctx, op.symbol->var_id));

        case IR_OPERAND_TEMP:
        fprintf(ctx->out, "    ldr w0, [sp, #%d]\n", get_temp_offset(ctx, op.temp));
    }
}



//===============


