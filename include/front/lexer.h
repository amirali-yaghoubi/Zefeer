#pragma once
#include "common/arena.h"


typedef enum {
    TOK_INT,

    TOK_LET,
    TOK_IF,
    TOK_ELSE,
    TOK_FOR,
    TOK_WHILE,
    TOK_FUNCTION,
    TOK_PRINT,

    TOK_IDENTIFIER,
    TOK_NUMBER,

    TOK_PLUS,
    TOK_MINUS,
    TOK_DIVIDE,
    TOK_MULTIPLY,

    TOK_LEFT_BRACE,
    TOK_RIGHT_BRACE,
    TOK_LEFT_PAREN,
    TOK_RIGHT_PAREN,
    TOK_COMMA,
    TOK_GREATER,
    TOK_LESS,
    TOK_GREATER_EQUAL,
    TOK_LESS_EQUAL,
    TOK_EQUAL_EQUAL,
    TOK_NOT_EQUAL,
    TOK_NOT,

    TOK_ASSIGNMENT,
    TOK_SEMICOLON,
    TOK_COLON,
    TOK_EOF,
    TOK_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    union {
        long int_value;
    } value;
    int line;
    char* file_name;
} Token;

typedef struct {
    const char* start;
    const char* current;
    int line;
    char* file_name;
} Lexer;


const char *token_type_to_str(TokenType t);
const char* token_to_str(Arena* a, const char* start, int len);
void lexer_init(Lexer* lexer, const char* file_name, const char* source);
Token get_next_token(Lexer* lexer);

