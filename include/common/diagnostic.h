#pragma once
#include "front/parser.h"


#define ERR_ASSIGN_BEFORE_DECLARATION "'%s' is not yet declared"
#define ERR_DOUBLE_DECLARATION "'%s' is already declared"
#define ERR_USE_BEFORE_ASSIGN "'%s' is not yet assigned to anything"
#define ERR_EXPECTED_TOKEN "expected token '%s' but got '%s'"
#define ERR_UNEXPECTED_TOKEN "unexpected token '%s'" 
#define ERR_INVALID_TYPE "invalid type"
#define ERR_MISSING_SEMICOLON "missing semicolon"
#define ERR_UNDEFINED_VARIABLE "the variable '%s' is undefined"
#define ERR_UNKNOWN_TOKEN "use of an unknown token"
#define ERR_INVALID_EXPRESSION_LEFT "invalid left side expression"
#define ERR_INVALID_EXPRESSION_RIGHT "invalid right side expression"
#define ERR_TYPE_MISMATCH "types for left and right side are mismatched"



#include <stdio.h>
#include <stdbool.h>


typedef enum {
    DIAG_ERROR,
    DIAG_WARNING
} DiagnosticType;


typedef struct {
    DiagnosticType type;
    bool has_error;
    long line;
    char* note;
    char* file_name;
} DiagnosticContext;


void diagnostic_report(DiagnosticContext* dc, const char* format, ...);