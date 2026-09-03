
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common/arena.h"
#include "front/lexer.h"
#include "front/parser.h"
#include "middle/semantic_analyzer.h"
#include "middle/ir.h"
#include "back/arm64.h"



char* read_file(Arena* a, const char* path);

int main()
{
    Arena reader_arena, parser_arena, semantic_analyzer_arena, ir_arena;
    
    arena_init(&reader_arena);
    arena_init(&parser_arena);
    arena_init(&semantic_analyzer_arena);
    arena_init(&ir_arena);


    const char* source_path = "test/arm64_test_source.zf";
    const char* output_file_name = "test/arm64_test_output.s";

    char* source = read_file(&reader_arena, source_path);
    if (!source)
    {
        fprintf(stderr, "Failed to read %s\n", source_path);
        return 1;
    }

    Lexer lexer = {0};
    lexer_init(&lexer, source_path, source);

    Parser parser = {0};
    parser_init(&parser, &lexer);

    SemanticContext ctx = {0};
    semantic_context_init(&ctx, &semantic_analyzer_arena);


    ASTNode* ast = parse_program(&parser_arena, &parser);
    if (!ast)
    {
        fprintf(stderr, "Parsing failed\n");
        return 1;
    }

    if(!analyze_program(ast, &ctx))
        return 1;
    
    
    IRContext irc = {0};
    ir_context_init(&irc, &ir_arena, ast, &ctx);

    ir_generate(&irc);
    
   arm64_generate(&irc, output_file_name);
   printf("Results in %s\n", output_file_name);


    
    arena_free(&reader_arena);
    arena_free(&parser_arena);
    arena_free(&semantic_analyzer_arena);

    ir_vec_free(&irc.instructions_vector);

    arena_free(&ir_arena);


    return 0;
}

char* read_file(Arena* a, const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file)
    {
        perror("fopen");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size_temp = ftell(file);
    if (size_temp < 0)
    {
        fclose(file);
        fprintf(stderr, "Error in reading %s\n", path);
        return NULL;
    }
    size_t size = (size_t)size_temp;
    rewind(file);

    char* buffer = arena_alloc(a, size + 1);
    if (!buffer)
    {
        fclose(file);
        fprintf(stderr, "Error in allocating memory from arena\n");
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}