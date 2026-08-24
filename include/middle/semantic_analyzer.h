#pragma once

#include "common/arena.h"


typedef enum {
    TYPE_INT,
    TYPE_VOID,
    TYPE_ERROR
} DataType;


typedef struct Symbol {
    char* name;
    DataType type;
    bool is_init;
    struct Symbol* next;
} Symbol;


typedef struct Scope {
    Symbol* symbols;
    struct Scope* parent;
} Scope;


typedef struct {
    Scope* current_scope;
    bool has_error;
    int error_count;
    int warning_count;
    Arena* arena;
} SemanticContext; 