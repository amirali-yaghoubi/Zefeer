#include "middle/semantic_analyzer.h"
#include "common/arena.h"
#include <stdbool.h>

void semantic_context_init(SemanticContext* ctx, Arena* a)
{
    ctx->arena = a;
    ctx->current_scope = NULL;
    ctx->has_error = false;
    ctx->error_count = 0;
    ctx->warning_count = 0;
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