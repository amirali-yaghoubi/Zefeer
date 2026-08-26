#include "middle/semantic_analyzer.h"
#include "common/arena.h"
#include "common/diagnostic.h"
#include "front/parser.h"
#include <stdio.h>
#include <stdbool.h>




static void push_scope(SemanticContext* ctx);
void semantic_context_init(SemanticContext* ctx, Arena* a)
{
    ctx->arena = a;
    ctx->current_scope = NULL;
    ctx->root_scope = NULL;
    ctx->has_error = false;
    ctx->error_count = 0;
    ctx->warning_count = 0;

    push_scope(ctx); // Creates global scope
    ctx->root_scope = ctx->current_scope; // Saves the global scope
    ctx->current_scope->var_count = 0;
}


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
    sym->var_id = ctx->current_scope->var_count; // 0, 1, 2...
    ctx->current_scope->var_count++;

    sym->next = ctx->current_scope->symbols;
    ctx->current_scope->symbols = sym;

    return true;
}

//  const char* token_to_str(Arena* a, const char* start, int len) as lexme

static void analyze_expression(ASTNode* node, SemanticContext* ctx)
{

}


static void analyze_declaration(ASTNode* node, SemanticContext* ctx)
{
    ASTVarDeclStmt* decl = (ASTVarDeclStmt*)node;

    const char* name = token_to_str(ctx->arena, decl->name_token.start, decl->name_token.length);
    
    if(!add_symbol(ctx, name, TYPE_INT))
    {
        DiagnosticContext dc = {
            .has_error = true,
            .line = decl->name_token.line,
            .note = NULL,
            .type = DIAG_ERROR,
            .file_name = decl->name_token.file_name
        };

        diagnostic_report(dc, ERR_DOUBLE_DECLARATION, name);
    }

    if(decl->initializer != NULL)
    {
        analyze_expression(decl->initializer, ctx);
        Symbol* sym =lookup_current(ctx, name);
        if(sym != NULL)
        {
        sym->is_init = true;
        }
    }
}


static void analyze_ASSIGNMENT(ASTNode* node, SemanticContext* ctx)
{

}


static void analyze_if(ASTNode* node, SemanticContext* ctx)
{

}


static void analyze_while(ASTNode* node, SemanticContext* ctx)
{

}


static void analyze_print(ASTNode* node, SemanticContext* ctx)
{

}



static void analyze_block(ASTNode* block, SemanticContext* ctx);
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


void analyze_program(ASTNode* root, SemanticContext* ctx)
{

}