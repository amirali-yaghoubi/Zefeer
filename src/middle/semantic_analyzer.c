#include "middle/semantic_analyzer.h"
#include "common/arena.h"
#include <stdbool.h>



static void push_scope(SemanticContext* ctx);
void semantic_context_init(SemanticContext* ctx, Arena* a)
{
    ctx->arena = a;
    ctx->current_scope = NULL;
    ctx->has_error = false;
    ctx->error_count = 0;
    ctx->warning_count = 0;

    push_scope(ctx); // Creates global scope
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

    sym->next = ctx->current_scope->symbols;
    ctx->current_scope->symbols = sym;

    return true;
}