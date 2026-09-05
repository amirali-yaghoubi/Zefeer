
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common/arena.h"
#include "common/file.h"
#include "front/lexer.h"
#include "front/parser.h"
#include "middle/semantic_analyzer.h"



int main()
{
    Arena reader_arena, parser_arena, semantic_analyzer_arena;
    
    arena_init(&reader_arena);
    arena_init(&parser_arena);
    arena_init(&semantic_analyzer_arena);


    arena_init(&reader_arena);
    const char* source_path = "test/semantic_analyzer_test_source.zf";

    char* source = read_file(&reader_arena, source_path);
    if (!source)
    {
        fprintf(stderr, "Failed to read %s\n", source_path);
        arena_free(&reader_arena);
        arena_free(&parser_arena);
        arena_free(&semantic_analyzer_arena);
        return 1;
    }

    Lexer lexer = {0};
    lexer_init(&lexer, source_path, source);

    Parser parser = {0};
    parser_init(&parser, &lexer);

    SemanticContext ctx = {0};
    semantic_context_init(&ctx, &semantic_analyzer_arena);


    ASTNode* ast = parse_program(&parser_arena, &parser);
    if (!ast) {
        fprintf(stderr, "Parsing failed\n");
        arena_free(&reader_arena);
        arena_free(&parser_arena);
        arena_free(&semantic_analyzer_arena);
        return 1;
    }

    printf("==========Source code==========\n\n%s\n\n===============================\n\n", source);
    if(!analyze_program(ast, &ctx))
    {
        fprintf(stderr, "Semantic analysis failed\n");
        arena_free(&reader_arena);
        arena_free(&parser_arena);
        arena_free(&semantic_analyzer_arena);
        return 1;
    }

    
    arena_free(&reader_arena);
    arena_free(&parser_arena);
    arena_free(&semantic_analyzer_arena);


    return 0;
}

