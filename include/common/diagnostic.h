#pragma once


#define ERR_ASSIGN_BEFORE_DECLARATION "'%s' is not yet declared"
#define ERR_DOUBLE_DECLARATION "'%s' is already declared"
#define ERR_USE_BEFORE_ASSIGN "'%s' is not yet assigned to anything"
#define ERR_EXPECTED_TOKEN "expected token '%s' but got '%s'"
#define ERR_INVALID_TYPE "invalid type"
#define ERR_MISSING_SEMICOLON "missing semicolon"



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
