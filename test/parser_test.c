
#include <stdio.h>
#include <stdlib.h>
#include "common/arena.h"
#include "front/lexer.h"
#include "front/parser.h"
#include "front/ast_visualizer.h"


char* read_file(Arena* a, const char* path);

int main()
{
    Arena reader_arena, parser_arena, visualizer_arena;
    
    arena_init(&reader_arena);
    arena_init(&parser_arena);
    arena_init(&visualizer_arena);

    arena_init(&reader_arena);
    const char* source_path = "test/parser_test_source.zf";

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


    ASTNode* ast = parse_program(&parser_arena, &parser);

    if (!ast) {
        fprintf(stderr, "Parsing failed\n");
        arena_free(&visualizer_arena);
        return 1;
    }

    printf("\n========== AST ==========\n\n");
    print_ast(ast, 0);
    printf("\n=========================\n");

    
    arena_free(&reader_arena);
    arena_free(&parser_arena);
    arena_free(&visualizer_arena);


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