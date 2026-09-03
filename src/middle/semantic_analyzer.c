#include "middle/semantic_analyzer.h"
#include "front/parser.h"
#include "common/arena.h"
#include "common/diagnostic.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>



static void push_scope(SemanticContext* ctx);
void semantic_context_init(SemanticContext* ctx, Arena* a)
{
    ctx->arena = a;
    ctx->current_scope = NULL;
    ctx->root_scope = NULL;
    ctx->has_error = false;
    ctx->error_count = 0;
    ctx->warning_count = 0;
    ctx->total_slots = 0;

    push_scope(ctx); // Creates global scope
    ctx->root_scope = ctx->current_scope; // Saves the global scope
    ctx->current_scope->var_count = 0;
}


// ====Helper Functions====
static void push_scope(SemanticContext* ctx)
{
    Scope* new_scope = arena_alloc(ctx->arena, sizeof(Scope));
    new_scope->symbols = NULL;
    new_scope->parent = ctx->current_scope;
    ctx->current_scope = new_scope;
}


static void pop_scope(SemanticContext* ctx)
{
    ctx->current_scope = ctx->current_scope->parent;
}


static Symbol* lookup_current(SemanticContext* ctx, const char* name)
{
    Symbol* current = ctx->current_scope->symbols;

    while(current != NULL)
    {
        if(strcmp(current->name, name) == 0)
            return current;
        
        current = current->next;
    }

    return NULL;
}


static Symbol* lookup_all(SemanticContext* ctx, const char* name)
{
    Scope* scope = ctx->current_scope;

    while(scope != NULL)
    {
        Symbol* current = scope->symbols;

        while(current != NULL)
        {
          if(strcmp(current->name, name) == 0)
            return current;
            
        current = current->next;
        }
        
        scope = scope->parent;
    }   
}


static bool add_symbol(SemanticContext* ctx, const char* name, DataType type)
{
    if(lookup_current(ctx, name) != NULL)
        return false;

    Symbol* sym = arena_alloc(ctx->arena, sizeof(Symbol));

    sym->name = name;
    sym->type = type;
    sym->is_init = false;
    sym->var_id = ctx->total_slots++; // 0, 1, 2...
    ctx->current_scope->var_count++;

    sym->next = ctx->current_scope->symbols;
    ctx->current_scope->symbols = sym;

    return true;
}
//==== ====




static void analyze_expression(ASTNode* node, SemanticContext* ctx);
static void analyze_binary_expr(ASTNode* node, SemanticContext* ctx)
{
    ASTBinaryExpr* bin = (ASTBinaryExpr*)node;

    analyze_expression(bin->lhs, ctx);
    analyze_expression(bin->rhs, ctx);

    if(bin->lhs->resolved_type == TYPE_ERROR) {
        DiagnosticContext dc = {
            .has_error = true,
            .note = NULL,
            .type = DIAG_ERROR,
            .line = (long)bin->op.line,
            .file_name = bin->op.file_name
        };
        diagnostic_report(&dc, ERR_INVALID_EXPRESSION_LEFT);
    } else if(bin->rhs->resolved_type == TYPE_ERROR) {
        DiagnosticContext dc = {
            .has_error = true,
            .note = NULL,
            .type = DIAG_ERROR,
            .line = (long)bin->op.line,
            .file_name = bin->op.file_name
        };
        diagnostic_report(&dc, ERR_INVALID_EXPRESSION_RIGHT);
    }
    bin->base.resolved_type = TYPE_INT; // must be changed if we gonna have more than 1 type later
}



static void analyze_number_expr(ASTNode* node)
{
    ASTNumberExpr* num = (ASTNumberExpr*)node;
    num->base.resolved_type = TYPE_INT; // must be changed if we gonna have more than 1 type later
}


static void analyze_ident_expr(ASTNode* node, SemanticContext* ctx)
{
    ASTIdentExpr* ident = (ASTIdentExpr*)node;

    const char* name = token_to_str(ctx->arena, ident->token.start, ident->token.length);

    Symbol* sym = lookup_all(ctx, name);

    if(sym == NULL) {
        ctx->has_error = true;

        DiagnosticContext dc = {
            .has_error = true,
            .type = DIAG_ERROR,
            .note = NULL,
            .line = (long)ident->token.line,
            .file_name = ident->token.file_name
        };

        diagnostic_report(&dc, ERR_UNDEFINED_VARIABLE, name);

        ident->symbol_ref = NULL;
        ident->base.resolved_type = TYPE_ERROR;
    } else if(sym->is_init == false) {
        ctx->has_error = true;

        DiagnosticContext dc = {
            .has_error = true,
            .type = DIAG_ERROR,
            .note = NULL,
            .line = (long)ident->token.line,
            .file_name = ident->token.file_name
        };

        diagnostic_report(&dc, ERR_USE_BEFORE_ASSIGN, name);
    } else {
        ident->symbol_ref = sym;
        ident->base.resolved_type = sym->type;
    }
}



static void analyze_expression(ASTNode* node, SemanticContext* ctx)
{
    switch(node->type)
    {
        case AST_EXPR_BINARY:
            analyze_binary_expr(node, ctx);
            break;

        case AST_EXPR_NUMBER:
            analyze_number_expr(node);
            break;

        case AST_EXPR_IDENT:
            analyze_ident_expr(node, ctx);
            break;
        default:
            break;
    }
}


static void analyze_declaration(ASTNode* node, SemanticContext* ctx)
{
    ASTVarDeclStmt* decl = (ASTVarDeclStmt*)node;

    const char* name = token_to_str(ctx->arena, decl->name_token.start, decl->name_token.length);
    
    if(!add_symbol(ctx, name, TYPE_INT))
    {
        ctx->has_error = true;

        DiagnosticContext dc = {
            .has_error = true,
            .line = decl->name_token.line,
            .note = NULL,
            .type = DIAG_ERROR,
            .file_name = decl->name_token.file_name
        };

        diagnostic_report(&dc, ERR_DOUBLE_DECLARATION, name);
    }

    if(decl->initializer != NULL)
    {
        analyze_expression(decl->initializer, ctx);
        Symbol* sym = lookup_current(ctx, name);
        if(sym != NULL)
        {
        sym->is_init = true;
        }
    }

    if(decl->type_token.type == TOK_INT) // could be switch if later decided to have other types too
        decl->base.resolved_type = TYPE_INT;
    
    Symbol* sym = lookup_current(ctx, name);
    decl->symbol_ref = sym;
}


static void analyze_ASSIGNMENT(ASTNode* node, SemanticContext* ctx)
{
    ASTAssignmentStmt* var = (ASTAssignmentStmt*)node;

    const char* name = token_to_str(ctx->arena, var->name_token.start, var->name_token.length);

    Symbol* sym = lookup_all(ctx, name);

    if(sym == NULL) {
        ctx->has_error = true;

        DiagnosticContext dc = {
            .has_error = true,
            .type = DIAG_ERROR,
            .note = NULL,
            .line = (long)var->name_token.line,
            .file_name = var->name_token.file_name
        };

        diagnostic_report(&dc, ERR_UNDEFINED_VARIABLE, name);
        var->base.resolved_type = TYPE_ERROR;
    }

    analyze_expression(var->value, ctx);

    if(var->base.resolved_type == TYPE_ERROR)
        return; // the following 'if' will work wrong with TYPE_ERROR
    if(var->base.resolved_type != var->value->resolved_type)
    {
        ctx->has_error = true;

        DiagnosticContext dc = {
            .has_error = true,
            .type = DIAG_ERROR,
            .note = NULL,
            .line = (long)var->name_token.line,
            .file_name = var->name_token.file_name
        };

        diagnostic_report(&dc,ERR_TYPE_MISMATCH);
        var->base.resolved_type = TYPE_ERROR;
    }

    var->symbol_ref = sym;
}


static void analyze_block(ASTNode* block, SemanticContext* ctx);
static void analyze_if(ASTNode* node, SemanticContext* ctx)
{
    ASTIfStmt* if_node = (ASTIfStmt*)node;

    analyze_expression(if_node->condition, ctx);

    analyze_block((ASTNode*)if_node->then_block, ctx);

    if(if_node->else_block != NULL)
        analyze_block((ASTNode*)if_node->else_block, ctx);
}


static void analyze_while(ASTNode* node, SemanticContext* ctx)
{
    ASTWhileStmt* while_node = (ASTWhileStmt*)node;

    analyze_expression(while_node->condition, ctx);

    analyze_block((ASTNode*)while_node->body, ctx);
}


static void analyze_print(ASTNode* node, SemanticContext* ctx)
{
    ASTPrintStmt* print_node = (ASTPrintStmt*)node;

    analyze_expression(print_node->expression, ctx);
}



static void analyze_statement(ASTNode* node, SemanticContext* ctx)
{
    switch(node->type)
    {
        case AST_STMT_VARDECL:
            analyze_declaration(node, ctx);
            break;
    
        case AST_STMT_ASSIGNMENT:
            analyze_ASSIGNMENT(node, ctx);
            break;

        case AST_STMT_IF:
            analyze_if(node, ctx);
            break;
        
        case AST_STMT_WHILE:
            analyze_while(node, ctx);
            break;

        case AST_STMT_PRINT:
            analyze_print(node, ctx);
            break;

        case AST_STMT_EXPR:
            analyze_expression(node, ctx);
            break;

        case AST_STMT_BLOCK:
            analyze_block(node, ctx);
            break;
        
        default:
            break;
    }

}


static void analyze_block(ASTNode* block, SemanticContext* ctx)
{
    push_scope(ctx);

    ASTBlock* block_node = (ASTBlock*)block;
    ASTBlockStmt* current = block_node->first;

    while(current != NULL)
    {
        analyze_statement(current->node, ctx);
         current = current->next;
    }

    pop_scope(ctx);
}


bool analyze_program(ASTNode* root, SemanticContext* ctx)
{
    analyze_block(root, ctx);
    
    if(ctx->has_error != 0)
        return false;
    
    return true;
}