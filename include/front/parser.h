#pragma once

#include "lexer.h"
#include "middle/semantic_analyzer.h"
#include "common/arena.h"
#include <stdlib.h>



typedef struct {
    Lexer* lexer;
    Token current;
} Parser;


typedef enum {
    AST_PROGRAM,

    //Statment
    AST_STMT_IF,
    AST_STMT_WHILE,
    AST_STMT_PRINT,
    AST_STMT_BLOCK,
    AST_STMT_RETURN,
    AST_STMT_VARDECL,
    AST_STMT_ASSIGNMENT,
    AST_STMT_EXPR,

    //Expression
    AST_EXPR_BINARY,
    AST_EXPR_UNARY,
    AST_EXPR_CALL,
    AST_EXPR_NUMBER,
    AST_EXPR_IDENT
} ASTType;


typedef struct ASTNode {
    ASTType type;
    DataType resolved_type;
} ASTNode;


typedef struct ASTBlockStmt {
    ASTNode* node;
    struct ASTBlockStmt* next;
} ASTBlockStmt;


typedef struct {
    ASTNode base;

    ASTNode** statements;
    int count;
} ASTProgram;


typedef struct {
    ASTNode base;
    Token token;
} ASTNumberExpr;


typedef struct {
    ASTNode base;
    Token token;
    Symbol* symbol_ref;
} ASTIdentExpr;


typedef struct {
    ASTNode base;

    ASTNode* lhs;
    ASTNode* rhs;

    Token op;
} ASTBinaryExpr;


typedef struct {
    ASTNode base;

    Token name_token;
    Token type_token;

    ASTNode* initializer;

    Symbol* symbol_ref;
} ASTVarDeclStmt;


typedef struct {
    ASTNode base;

    Token name_token;
    ASTNode* value;

    Symbol* symbol_ref;
} ASTAssignmentStmt;


typedef struct {
    ASTNode base;
    ASTBlockStmt* first;
} ASTBlock;


typedef struct {
    ASTNode base;

    ASTNode* condition;
    ASTBlock* then_block;
    ASTBlock* else_block;
} ASTIfStmt;


typedef struct {
    ASTNode base;

    ASTNode* condition;
    ASTBlock* body;
} ASTWhileStmt;



typedef struct {
    ASTNode base;

    ASTNode* expression;
} ASTPrintStmt;



const char *ASTTypeStr(ASTType type);
void parser_init(Parser* p, Lexer* l);
ASTNode* parse_program(Arena* a, Parser* p);