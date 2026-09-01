#include "middle/ir.h"
#include "common/arena.h"
#include <stdlib.h>
#include <string.h>





//====Vector====
static void ir_vec_resize(IRInstructionVec *vec, size_t new_cap)
{
    IRInstruction *new_data = realloc(vec->data, new_cap * sizeof(IRInstruction));
    if (!new_data) return;
    vec->data = new_data;
    vec->capacity = new_cap;
}

static void ir_vec_init(IRInstructionVec *vec)
{
    vec->size = 0;
    vec->capacity = 4;
    vec->data = malloc(vec->capacity * sizeof(IRInstruction));
}

static void ir_vec_push(IRInstructionVec *vec, IRInstruction val)
{
    if (vec->size == vec->capacity) {
        ir_vec_resize(vec, (int)(vec->capacity * 1.4));
    }
    vec->data[vec->size++] = val;
}

static IRInstruction ir_vec_pop(IRInstructionVec *vec)
{
    return vec->data[--vec->size];
}

static IRInstruction *ir_vec_get(IRInstructionVec *vec, size_t index)
{
    return &vec->data[index];
}

void ir_vec_free(IRInstructionVec *vec)
{
    free(vec->data);
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
}
//====      ====


void ir_context_init(IRContext* irc, Arena* a, ASTNode* ast)
{
    irc->arena = a;
    ir_vec_init(&irc->instructions_vector);
    irc->next_temp = 0;
    irc->next_label = 0;
    irc->ast = ast;
}


//====Helpers====
static IROperand* make_operand_const(IRContext* irc, int val)
{
    IROperand* op = arena_alloc(irc->arena, sizeof(IROperand));

    op->type = IR_OPERAND_CONST;
    op->value = val;

    return op;
}


static IROperand* make_operand_var(IRContext* irc, Symbol* sym)
{
    IROperand* op = arena_alloc(irc->arena, sizeof(IROperand));

    op->type = IR_OPERAND_VAR;
    op->symbol = sym;

    return op;
}


static IROperand* make_operand_temp(IRContext* irc, int temp)
{
    IROperand* op = arena_alloc(irc->arena, sizeof(IROperand));

    op->type = IR_OPERAND_TEMP;
    op->temp = temp;

    return op;
}


//====       ====

static generate_expresssion(ASTNode* expr)
{
    switch(expr->type)
    {
        case AST_EXPR_NUMBER :
            generate_expression_number();
            break;

        case AST_EXPR_BINARY :
            generate_expresssion_binary();
            break;

        case AST_EXPR_IDENT :
            generate_expresssion_ident();
            break;
    }
}


static generate_statement(ASTNode* stmt)
{
    switch(stmt->type)
    {
        case AST_STMT_VARDECL:
            generate_declaration();
            break;

        case AST_STMT_ASSIGNMENT:
            generate_assignment();
            break;

        case AST_STMT_IF:
            generate_if();
            break;

        case AST_STMT_WHILE:
            generate_while();
            break;

        case AST_STMT_PRINT:
            generate_print();
            break;

        case AST_STMT_EXPR:
            generate_expression();
            break;

        case AST_STMT_BLOCK:
            //???
            break;
    }
}
