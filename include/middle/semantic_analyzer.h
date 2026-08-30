#pragma once

#include "common/arena.h"
#include "front/parser.h"
#include <stdbool.h>

typedef enum {
    TYPE_INT,
    TYPE_VOID,
    TYPE_ERROR
} DataType;


typedef struct Symbol {
    char* name;
    DataType type;
    bool is_init;
    int var_id;
    struct Symbol* next;
} Symbol;


typedef struct Scope {
    Symbol* symbols;
    int var_count;
    struct Scope* parent;
} Scope;


typedef struct {
    Scope* current_scope;
    Scope* root_scope;
    bool has_error;
    int error_count;
    int warning_count;
    int total_slots;
    Arena* arena;
} SemanticContext;


void semantic_context_init(SemanticContext* ctx, Arena* a);
bool analyze_program(ASTNode* root, SemanticContext* ctx);
