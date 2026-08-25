#include "front/ast_visualizer.h"
#include <stdio.h>

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) printf("  ");
}

static void print_token_summary(Token t) {
    printf("'%.*s'", t.length, t.start);
}

void print_ast(ASTNode* node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("(null)\n");
        return;
    }

    print_indent(indent);

    switch (node->type) {
        case AST_PROGRAM: {
            ASTProgram* prog = (ASTProgram*)node;
            printf("AST_PROGRAM (statements=%d)\n", prog->count);
            for (int i = 0; i < prog->count; i++) {
                print_ast(prog->statements[i], indent + 1);
            }
            break;
        }

        case AST_STMT_VARDECL: {
            ASTVarDeclStmt* decl = (ASTVarDeclStmt*)node;
            printf("AST_STMT_VARDECL name=");
            print_token_summary(decl->name_token);
            printf(" type=");
            print_token_summary(decl->type_token);
            printf("\n");
            if (decl->initializer) {
                print_indent(indent + 1);
                printf("initializer:\n");
                print_ast(decl->initializer, indent + 2);
            }
            break;
        }

        case AST_STMT_ASSIGNMENT: {
            ASTAssignmentStmt* assign = (ASTAssignmentStmt*)node;
            printf("AST_STMT_ASSIGNMENT name=");
            print_token_summary(assign->name_token);
            printf("\n");
            print_indent(indent + 1);
            printf("value:\n");
            print_ast(assign->value, indent + 2);
            break;
        }

        case AST_STMT_IF: {
            ASTIfStmt* ifstmt = (ASTIfStmt*)node;
            printf("AST_STMT_IF\n");
            print_indent(indent + 1);
            printf("condition:\n");
            print_ast(ifstmt->condition, indent + 2);
            print_indent(indent + 1);
            printf("then:\n");
            print_ast((ASTNode*)ifstmt->then_block, indent + 2);
            if (ifstmt->else_block) {
                print_indent(indent + 1);
                printf("else:\n");
                print_ast((ASTNode*)ifstmt->else_block, indent + 2);
            }
            break;
        }

        case AST_STMT_WHILE: {
            ASTWhileStmt* whilestmt = (ASTWhileStmt*)node;
            printf("AST_STMT_WHILE\n");
            print_indent(indent + 1);
            printf("condition:\n");
            print_ast(whilestmt->condition, indent + 2);
            print_indent(indent + 1);
            printf("body:\n");
            print_ast((ASTNode*)whilestmt->body, indent + 2);
            break;
        }

        case AST_STMT_BLOCK: {
            ASTBlock* block = (ASTBlock*)node;
            printf("AST_STMT_BLOCK\n");
            ASTBlockStmt* cur = block->first;
            while (cur) {
                print_ast(cur->node, indent + 1);
                cur = cur->next;
            }
            break;
        }

        case AST_STMT_PRINT: {
            ASTPrintStmt* printstmt = (ASTPrintStmt*)node;
            printf("AST_STMT_PRINT\n");
            print_indent(indent + 1);
            printf("expression:\n");
            print_ast(printstmt->expression, indent + 2);
            break;
        }

        case AST_EXPR_NUMBER: {
            ASTNumberExpr* num = (ASTNumberExpr*)node;
            printf("AST_EXPR_NUMBER value=%ld\n", num->token.value.int_value);
            break;
        }

        case AST_EXPR_IDENT: {
            ASTIdentExpr* id = (ASTIdentExpr*)node;
            printf("AST_EXPR_IDENT name=");
            print_token_summary(id->token);
            printf("\n");
            break;
        }

        case AST_EXPR_BINARY: {
            ASTBinaryExpr* bin = (ASTBinaryExpr*)node;
            printf("AST_EXPR_BINARY op=");
            print_token_summary(bin->op);
            printf("\n");
            print_indent(indent + 1);
            printf("lhs:\n");
            print_ast(bin->lhs, indent + 2);
            print_indent(indent + 1);
            printf("rhs:\n");
            print_ast(bin->rhs, indent + 2);
            break;
        }

        case AST_EXPR_UNARY: {
            // Unary is defined in parser.h but not yet implemented in parser.c
            printf("AST_EXPR_UNARY (not implemented)\n");
            break;
        }

        case AST_EXPR_CALL: {
            // Call is defined but not yet implemented
            printf("AST_EXPR_CALL (not implemented)\n");
            break;
        }

        case AST_STMT_RETURN:
            printf("AST_STMT_RETURN (not implemented)\n");
            break;

        case AST_STMT_EXPR:
            printf("AST_STMT_EXPR (not implemented)\n");
            break;

        default:
            printf("UNKNOWN AST NODE (type=%d)\n", node->type);
            break;
    }
}