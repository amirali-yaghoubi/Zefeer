#include "frontend/lexer.h"
#include "common/arena.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



static void lexer_set(
        const char* start,
        const char* current,
        int line,
        Lexer* lexer
        )
{
    lexer->start = start;
    lexer->current = current;
    lexer->line = line;
}


void lexer_init(Lexer* lexer, const char* source)
{
    lexer_set(source, source, 1, lexer);
}


//Just a simple helper for printing the TokenType(s) and debug
const char *token_type_to_str(TokenType t) {
    switch (t) {
        case TOK_INT:            return "TOK_INT";

        case TOK_LET:            return "TOK_LET";
        case TOK_IF:             return "TOK_IF";
        case TOK_ELSE:           return "TOK_ELSE";
        case TOK_FOR:            return "TOK_FOR";
        case TOK_WHILE:          return "TOK_WHILE";
        case TOK_FUNCTION:       return "TOK_FUNCTION";
        case TOK_PRINT:          return "TOK_PRINT";

        case TOK_IDENTIFIER:     return "TOK_IDENTIFIER";
        case TOK_NUMBER:         return "TOK_NUMBER";

        case TOK_PLUS:           return "TOK_PLUS";
        case TOK_MINUS:          return "TOK_MINUS";
        case TOK_DIVIDE:         return "TOK_DIVIDE";
        case TOK_MULTIPLY:       return "TOK_MULTIPLY";

        case TOK_LEFT_BRACE:     return "TOK_LEFT_BRACE";
        case TOK_RIGHT_BRACE:    return "TOK_RIGHT_BRACE";
        case TOK_LEFT_PAREN:     return "TOK_LEFT_PAREN";
        case TOK_RIGHT_PAREN:    return "TOK_RIGHT_PAREN";
        case TOK_COMMA:           return "TOK_COMMA";
        case TOK_GREATER:        return "TOK_GREATER";
        case TOK_LESS:           return "TOK_LESS";
        case TOK_GREATER_EQUAL:  return "TOK_GREATER_EQUAL";
        case TOK_LESS_EQUAL:     return "TOK_LESS_EQUAL";
        case TOK_EQUAL_EQUAL:    return "TOK_EQUAL_EQUAL";
        case TOK_NOT_EQUAL:      return "TOK_NOT_EQUAL";
        case TOK_NOT:            return "TOK_NOT";

        case TOK_ASSIGNMENT:     return "TOK_ASSIGNMENT";
        case TOK_SEMICOLON:      return "TOK_SEMICOLON";
        case TOK_COLON:          return "TOK_COLON";
        case TOK_EOF:            return "TOK_EOF";
        case TOK_UNKNOWN:        return "TOK_UNKNOWN";

        default:                 return "INVALID_TOKEN";
    }
}



const char* token_to_str(Arena* a, const char* start, int len)
{
    char* str = arena_alloc(a, len + 1);

    memcpy(str, start, len);
    str[len] = '\0';

    return str;
}



static TokenType check_keyword(const char* start, int length)
{
    if (length == 3 && strncmp(start, "let", 3) == 0)
        return TOK_LET;

    if (length == 2 && strncmp(start, "if", 2) == 0)
        return TOK_IF;

    if (length == 4 && strncmp(start, "else", 4) == 0)
        return TOK_ELSE;

    if (length == 3 && strncmp(start, "for", 3) == 0)
        return TOK_FOR;

    if (length == 5 && strncmp(start, "while", 5) == 0)
        return TOK_WHILE;

     if (length == 5 && strncmp(start, "print", 5) == 0)
        return TOK_PRINT;

    if (length == 4 && strncmp(start, "func", 4) == 0)
        return TOK_FUNCTION;

    if (length == 3 && strncmp(start, "int", 3) == 0)
        return TOK_INT;

    return TOK_IDENTIFIER;
}


static void skip_white_space(Lexer* l)
{
    while (isspace((unsigned char)*l->current))
    {
        //updating the line
        if (*l->current == '\n')
            l->line++;
        l->current++;
    }
}


static Token lex_math_oprators(Lexer* l, const char* start)
{
    if (*start == '+')
    {
        l->current++;
        return (Token){
            .type = TOK_PLUS,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == '-')
    {
        l->current++;
        return (Token){
            .type = TOK_MINUS,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == '/')
    {
        l->current++;
        return (Token){
            .type = TOK_DIVIDE,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == '*')
    {
        l->current++;
        return (Token){
            .type = TOK_MULTIPLY,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    return (Token){
        .type = TOK_UNKNOWN, 
        .start = start,
        .length = 1,
        .line = l->line
    };
}


static Token lex_punctuations(Lexer* l, const char* start)
{
    if (*start == ':')
    {
        l->current++;
        return (Token){
            .type = TOK_COLON,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == ';')
    {
        l->current++;
        return (Token){
            .type = TOK_SEMICOLON,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == ',')
    {
        l->current++;
        return (Token){
            .type = TOK_COMMA,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }


    if (*start == '{')
    {
        l->current++;
        return (Token){
            .type = TOK_LEFT_BRACE,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == '}')
    {
        l->current++;
        return (Token){
            .type = TOK_RIGHT_BRACE,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == '(')
    {
        l->current++;
        return (Token){
            .type = TOK_LEFT_PAREN,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == ')')
    {
        l->current++;
        return (Token){
            .type = TOK_RIGHT_PAREN,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    return (Token){
        .type = TOK_UNKNOWN, 
        .start = start,
        .length = 1,
        .line = l->line
    };
}

//Also includes NOT token(!)
static Token lex_assignment_and_compare(Lexer* l, const char* start)
{
    if (*start == '>')
    { 
        l->current++;
        if (*l->current == '=')
        {
            l->current++;
            return (Token){
                .type = TOK_GREATER_EQUAL,
                .start = start,
                .length = 2,
                .line = l->line
            };
        }
        return (Token){
            .type = TOK_GREATER,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == '<')
    { 
        l->current++;
        if (*l->current == '=')
        {
            l->current++;
            return (Token){
                .type = TOK_LESS_EQUAL,
                .start = start,
                .length = 2,
                .line = l->line
            };
        }
        return (Token){
            .type = TOK_LESS,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    if (*start == '=')
    { 
        l->current++;
        if (*l->current == '=')
        {
            l->current++;
            return (Token){
                .type = TOK_EQUAL_EQUAL,
                .start = start,
                .length = 2,
                .line = l->line
            };
        }
        return (Token){
            .type = TOK_ASSIGNMENT,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    //! and !=
    if (*start == '!')
    { 
        l->current++;
        if (*l->current == '=')
        {
            l->current++;
            return (Token){
                .type = TOK_NOT_EQUAL,
                .start = start,
                .length = 2,
                .line = l->line
            };
        }
        return (Token){
            .type = TOK_NOT,
            .start = start,
            .length = 1,
            .line = l->line
        };
    }

    return (Token){
        .type = TOK_UNKNOWN, 
        .start = start,
        .length = 1,
        .line = l->line
    };
}


static Token lex_identifier_or_keyword(Lexer* l, const char* start)
{
    if (isalpha((unsigned char)*start))
    {
        while (isalnum((unsigned char)*l->current) || *l->current == '_')
            l->current++;
        int length = (int)(l->current - start);
        return (Token){
            .type = check_keyword(start, length),
            .start = start,
            .length = length,
            .line = l->line
        };
    }

    return (Token){
        .type = TOK_UNKNOWN, 
        .start = start,
        .length = 1,
        .line = l->line
    };
}


static Token lex_number(Lexer* l, const char* start)
{
    //Only intiger for now
    if (isdigit((unsigned char)*start))
    {
        long value = 0;
        while (isdigit((unsigned char)*l->current))
        {
            int digit = *l->current - '0';
            value = value * 10 + digit;
            l->current++;
        }
        return (Token){
            .type = TOK_NUMBER,
            .start = start,
            .length = (int)(l->current - start),
            .value.int_value = value,
            .line = l->line
        };
    }

    return (Token){
        .type = TOK_UNKNOWN, 
        .start = start,
        .length = 1,
        .line = l->line
    };
}


Token get_next_token(Lexer* l)
{
    skip_white_space(l);

    const char* start = l->current;

    if (*start == '\0')
    {
        return (Token){
            .type = TOK_EOF,
            .start = start,
            .length = 0,
            .line = l->line
        };
    }

    Token tok;

    tok = lex_math_oprators(l, start);
    if (tok.type != TOK_UNKNOWN)
        return tok;

    tok = lex_punctuations(l, start);
    if (tok.type != TOK_UNKNOWN)
        return tok;

    tok = lex_assignment_and_compare(l, start);
    if (tok.type != TOK_UNKNOWN)
        return tok;

    tok = lex_identifier_or_keyword(l, start);
    if (tok.type != TOK_UNKNOWN)
        return tok;

    tok = lex_number(l, start);
    if (tok.type != TOK_UNKNOWN)
        return tok;

    l->current++;
    return (Token){
        .type = TOK_UNKNOWN,
        .start = start,
        .length = 1,
        .line = l->line
    };
}

