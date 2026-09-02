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


static int new_temp(IRContext* irc)
{
    return irc->next_temp++;
}


static unsigned new_label(IRContext* irc)
{
    return irc->next_label++;
}


static void emit_inst(IRContext* irc, IROpCode op, IROperand dst, IROperand src1, IROperand src2, unsigned label)
{
    IRInstruction inst = {op, dst, src1, src2, label};
    ir_vec_push(&irc->instructions_vector, inst);
}


static void emit_store(IRContext* irc, ASTNode* expr, Symbol* sym)
{
    IROperand* rhs = generate_expression(irc, expr);
    IROperand* lhs = make_operand_var(irc, sym);
    emit_inst(irc, IR_MOVE, *lhs, *rhs, (IROperand){0}, 0);
}
//====    ====





//====Expression Helpers====

static IROperand* generate_expression_number(IRContext* irc, ASTNode* node)
{
    ASTNumberExpr* num = (ASTNumberExpr*)node;
    return make_operand_const(irc, num->token.value.int_value);
}


static IROperand* generate_expression_ident(IRContext* irc, ASTNode* node)
{
    ASTIdentExpr* ident = (ASTIdentExpr*)node;
    return make_operand_var(irc, ident->symbol_ref);
}

static IROperand* generate_expression(IRContext* irc, ASTNode* expr);
static IROperand* generate_expression_binary(IRContext* irc, ASTNode* node)
{
    ASTBinaryExpr* bin = (ASTBinaryExpr*)node;

    IROperand* left = generate_expression(irc, bin->lhs);
    IROperand* right = generate_expression(irc, bin->rhs);

    int t = new_temp(irc);
    IROperand* dst = make_operand_temp(irc, t);

    IROpCode opcode;
    switch(bin->op.type)
    {
        case TOK_PLUS : opcode = IR_ADD; break;
        case TOK_MINUS : opcode = IR_SUB; break;
        case TOK_MULTIPLY : opcode = IR_MUL; break;
        case TOK_DIVIDE : opcode = IR_DIV; break;

        case TOK_EQUAL_EQUAL : opcode = IR_CMP_EQ; break;
        case TOK_NOT_EQUAL : opcode = IR_CMP_NE; break;
        case TOK_GREATER_EQUAL : opcode = IR_CMP_GE; break;
        case TOK_LESS_EQUAL : opcode = IR_CMP_LE; break;
        case TOK_GREATER : opcode = IR_CMP_GT; break;
        case TOK_LESS : opcode = IR_CMP_LT; break;
    }
    emit_inst(irc, opcode, *dst, *left, *right, 0);
    return dst;
}
//=================


//====Statemente Helpers====
static void generate_declaration(IRContext* irc, ASTNode* node)
{
    ASTVarDeclStmt* decl = (ASTVarDeclStmt*)node;
    if(decl->initializer)
        emit_store(irc, decl->initializer, decl->symbol_ref);
}

static void generate_assignment(IRContext* irc, ASTNode* node)
{
    ASTAssignmentStmt* assign = (ASTAssignmentStmt*)node;
    emit_store(irc, assign->value, assign->symbol_ref);
}

static void generate_if(IRContext* irc, ASTNode* node)
{
    ASTIfStmt* if_stmt = (ASTIfStmt*)node;
    IROperand* cond = generate_expression(irc, if_stmt->condition);

    unsigned true_label = new_label(irc);
    unsigned false_label = new_label(irc);
    unsigned end_label = new_label(irc);

    emit_inst(irc, IR_BRANCH, (IROperand){0}, *cond, (IROperand){0}, true_label);
    emit_inst(irc, IR_JUMP, (IROperand){0}, (IROperand){0}, (IROperand){0}, false_label);

    emit_inst(irc, IR_LABEL, (IROperand){0}, (IROperand){0}, (IROperand){0}, true_label);
    generate_statement(irc, (ASTNode*)if_stmt->then_block);

    if(if_stmt->else_block) {
        emit_inst(irc, IR_JUMP, (IROperand){0}, (IROperand){0}, (IROperand){0}, end_label);
        emit_inst(irc, IR_LABEL, (IROperand){0}, (IROperand){0}, (IROperand){0}, false_label);
        generate_statement(irc, if_stmt->else_block);
        emit_inst(irc, IR_LABEL, (IROperand){0}, (IROperand){0}, (IROperand){0}, end_label);
    } else {
        emit_inst(irc, IR_LABEL, (IROperand){0}, (IROperand){0}, (IROperand){0}, false_label);
    }
}

static void generate_while(IRContext* irc, ASTNode* node)
{
    ASTWhileStmt* while_stmt = (ASTWhileStmt*)node;

     unsigned loop_label = new_label(irc);
     unsigned exit_label = new_label(irc);

     emit_inst(irc, IR_LABEL, (IROperand){0}, (IROperand){0}, (IROperand){0}, loop_label);

     IROperand* cond = generate_expression(irc, while_stmt->condition);

     IROperand* zero = make_operand_const(irc, 0);
     int eq_temp = mew_temp(irc);
    IROperand* eq_op = make_operand_temp(irc, eq_temp);

    emit_inst(irc, IR_CMP_EQ, *eq_op, *cond, *zero, 0);

    generate_statement(irc, while_stmt->body);
    emit_inst(irc, IR_JUMP, (IROperand){0}, (IROperand){0}, (IROperand){0}, loop_label);
    emit_inst(irc, IR_LABEL, (IROperand){0}, (IROperand){0}, (IROperand){0}, exit_label);
}


static void generate_print(IRContext* irc, ASTNode* node)
{
    ASTPrintStmt* print_stmt = (ASTPrintStmt*)node;
    IROperand* val = generate_expression(irc, print_stmt->expression);
    emit_inst(irc, IR_PRINT, (IROperand){0}, *val, (IROperand){0}, 0);
}


static void generate_block(IRContext* irc, ASTNode* node)
{
    ASTBlock* block = (ASTBlock*)node;
    ASTBlockStmt* current = block->first;
    while(current)
    {
        generate_statement(irc, current->node);
        current = current->next;
    }
}
//==========================


//Expression dispatcher
static IROperand* generate_expression(IRContext* irc, ASTNode* expr)
{
    switch(expr->type)
    {
        case AST_EXPR_NUMBER:
            return generate_expression_number(irc, expr);
            break;

        case AST_EXPR_IDENT:
            return generate_expression_ident(irc, expr);
            break;

        case AST_EXPR_BINARY:
            return generate_expression_binary(irc, expr);
            break;
    }
}


//Statement dispatcher
static void generate_statement(IRContext* irc, ASTNode* stmt)
{
    switch(stmt->type)
    {
        case AST_STMT_VARDECL:
            generate_declaration(irc, stmt);
            break;

        case AST_STMT_ASSIGNMENT:
            generate_assignment(irc, stmt);
            break;

        case AST_STMT_IF:
            generate_if(irc, stmt);
            break;

        case AST_STMT_WHILE:
            generate_while(irc, stmt);
            break;

        case AST_STMT_PRINT:
            generate_print(irc, stmt);
            break;

        case AST_STMT_EXPR:
            generate_expression(irc, stmt);
            break;

        case AST_STMT_BLOCK:
            generate_block(irc, stmt);
            break;
    }
}


void ir_generate(IRContext* irc)
{
    generate_statement(irc, irc->ast);
}