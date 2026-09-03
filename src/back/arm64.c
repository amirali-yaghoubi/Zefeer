#include "back/arm64.h"
#include "back/arm64.h"
#include "middle/ir.h"
#include "middle/semantic_analyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//====Helpers====

static int get_var_offset(int var_id)
{
    return var_id * 4;
}

static int get_temp_offset(ARM64Context* ctx, int temp_id)
{
    return (ctx->num_vars + temp_id) * 4;
}


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


static void emit_load_operand(ARM64Context* ctx, IROperand op) 
{
    switch (op.type) {
        case IR_OPERAND_CONST:
            fprintf(ctx->out, "    mov w0, #%d\n", op.value);
            break;
        case IR_OPERAND_VAR:
            fprintf(ctx->out, "    ldr w0, [sp, #%d]\n", get_var_offset(op.symbol->var_id));
            break;
        case IR_OPERAND_TEMP:
            fprintf(ctx->out, "    ldr w0, [sp, #%d]\n", get_temp_offset(ctx, op.temp));
            break;
        default:
            break;
    }
}

static void emit_load_operand_to_w1(ARM64Context* ctx, IROperand op)
{
    switch (op.type) {
        case IR_OPERAND_CONST:
            fprintf(ctx->out, "    mov w1, #%d\n", op.value);
            break;
        case IR_OPERAND_VAR:
            fprintf(ctx->out, "    ldr w1, [sp, #%d]\n", get_var_offset(op.symbol->var_id));
            break;
        case IR_OPERAND_TEMP:
            fprintf(ctx->out, "    ldr w1, [sp, #%d]\n", get_temp_offset(ctx, op.temp));
            break;
        default:
            break;
    }
}

static void emit_store_dest(ARM64Context* ctx, IROperand dst)
{
    switch (dst.type) {
        case IR_OPERAND_VAR:
            fprintf(ctx->out, "    str w0, [sp, #%d]\n", get_var_offset(dst.symbol->var_id));
            break;
        case IR_OPERAND_TEMP:
            fprintf(ctx->out, "    str w0, [sp, #%d]\n", get_temp_offset(ctx, dst.temp));
            break;
        default:
            break;
    }
}

//===================


// Main generate function
void arm64_generate(IRContext* irc, const char* filename)
{
    // ---- 1. Compute max temp ----
    int max_temp = 0;
    for (size_t i = 0; i < irc->instructions_vector.size; i++) {
        IRInstruction* inst = &irc->instructions_vector.data[i];
        if (inst->dst.type == IR_OPERAND_TEMP && inst->dst.temp > max_temp)
            max_temp = inst->dst.temp;
        if (inst->src_1.type == IR_OPERAND_TEMP && inst->src_1.temp > max_temp)
            max_temp = inst->src_1.temp;
        if (inst->src_2.type == IR_OPERAND_TEMP && inst->src_2.temp > max_temp)
            max_temp = inst->src_2.temp;
    }

    // ---- 2. Create context ----
    ARM64Context ctx;
    ctx.irc = irc;
    ctx.num_vars = irc->ctx->total_slots;
    ctx.max_temp = max_temp;
    ctx.total_slots = ctx.num_vars + ctx.max_temp + 1;
    ctx.frame_size = ((ctx.total_slots * 4) + 15) & ~15;

    ctx.out = fopen(filename, "w");
    if (!ctx.out) {
        fprintf(stderr, "Could not open output file %s\n", filename);
        return;
    }

    // ---- 3. Header ----
    fprintf(ctx.out, "    .global main\n");
    fprintf(ctx.out, "    .text\n");
    fprintf(ctx.out, "main:\n");

    emit_prologue(&ctx);

    // ---- 4. Emit instructions ----
    for (size_t i = 0; i < irc->instructions_vector.size; i++) {
        IRInstruction* inst = &irc->instructions_vector.data[i];

        switch (inst->op_code) {
            case IR_MOVE:
                emit_load_operand(&ctx, inst->src_1);
                emit_store_dest(&ctx, inst->dst);
                break;

            case IR_ADD:
                emit_load_operand(&ctx, inst->src_1);
                emit_load_operand_to_w1(&ctx, inst->src_2);
                fprintf(ctx.out, "    add w0, w0, w1\n");
                emit_store_dest(&ctx, inst->dst);
                break;

            case IR_SUB:
                emit_load_operand(&ctx, inst->src_1);
                emit_load_operand_to_w1(&ctx, inst->src_2);
                fprintf(ctx.out, "    sub w0, w0, w1\n");
                emit_store_dest(&ctx, inst->dst);
                break;

            case IR_MUL:
                emit_load_operand(&ctx, inst->src_1);
                emit_load_operand_to_w1(&ctx, inst->src_2);
                fprintf(ctx.out, "    mul w0, w0, w1\n");
                emit_store_dest(&ctx, inst->dst);
                break;

            case IR_DIV:
                emit_load_operand(&ctx, inst->src_1);
                emit_load_operand_to_w1(&ctx, inst->src_2);
                fprintf(ctx.out, "    sdiv w0, w0, w1\n");
                emit_store_dest(&ctx, inst->dst);
                break;

            case IR_CMP_EQ:
            case IR_CMP_NE:
            case IR_CMP_LT:
            case IR_CMP_GT:
            case IR_CMP_LE:
            case IR_CMP_GE: {
                const char* cond = "";
                switch (inst->op_code) {
                    case IR_CMP_EQ: cond = "eq"; break;
                    case IR_CMP_NE: cond = "ne"; break;
                    case IR_CMP_LT: cond = "lt"; break;
                    case IR_CMP_GT: cond = "gt"; break;
                    case IR_CMP_LE: cond = "le"; break;
                    case IR_CMP_GE: cond = "ge"; break;
                    default: break;
                }
                emit_load_operand(&ctx, inst->src_1);
                emit_load_operand_to_w1(&ctx, inst->src_2);
                fprintf(ctx.out, "    cmp w0, w1\n");
                fprintf(ctx.out, "    mov w0, #0\n");
                fprintf(ctx.out, "    mov w1, #1\n");
                fprintf(ctx.out, "    csel w0, w1, w0, %s\n", cond);
                emit_store_dest(&ctx, inst->dst);
                break;
            }

            case IR_BRANCH:
                emit_load_operand(&ctx, inst->src_1);
                fprintf(ctx.out, "    cmp w0, #0\n");
                fprintf(ctx.out, "    bne L%d\n", inst->label);
                break;

            case IR_JUMP:
                fprintf(ctx.out, "    b L%d\n", inst->label);
                break;

            case IR_LABEL:
                fprintf(ctx.out, "L%d:\n", inst->label);
                break;

            case IR_PRINT: {
                IROperand op = inst->src_1;
                switch (op.type) {
                    case IR_OPERAND_CONST:
                        fprintf(ctx.out, "    mov w1, #%d\n", op.value);
                        break;
                    case IR_OPERAND_VAR:
                        fprintf(ctx.out, "    ldr w1, [sp, #%d]\n", get_var_offset(op.symbol->var_id));
                        break;
                    case IR_OPERAND_TEMP:
                        fprintf(ctx.out, "    ldr w1, [sp, #%d]\n", get_temp_offset(&ctx, op.temp));
                        break;
                    default:
                        break;
                }
                fprintf(ctx.out, "    ldr x0, =fmt\n");
                fprintf(ctx.out, "    bl printf\n");
                break;
            }

            default:
                break;
        }
    }

    // ---- 5. Epilogue and data ----
    emit_epilogue(&ctx);
    fprintf(ctx.out, ".data\n");
    fprintf(ctx.out, "fmt: .string \"%%d\\n\"\n");

    fclose(ctx.out);
}