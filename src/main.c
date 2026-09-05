#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>   
#include "common/arena.h"
#include "common/file.h"
#include "front/lexer.h"
#include "front/parser.h"
#include "middle/semantic_analyzer.h"
#include "middle/ir.h"
#include "back/arm64.h"



int main(int argc, char **argv) {
    const char *source_path = NULL;
    char *output_file_name = NULL;   // dynamically allocated

    if (argc != 2 && argc != 4) {
        printf("Usage:\n");
        printf("  %s source.zf\n", argv[0]);
        printf("  %s -o output.s source.zf\n", argv[0]);
        printf("  %s source.zf -o output.s\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        // Default mode: source.zf → source.s
        source_path = argv[1];

        int src_len = strlen(source_path);
        // We'll validate .zf later, but we need the base length now.
        // If src_len < 3, base_len becomes negative – we cap it at 0.
        int base_len = (src_len >= 3) ? (src_len - 3) : 0;

        // Allocate: base_len + 2 (for ".s") + 1 (for '\0')
        output_file_name = malloc(base_len + 3);
        if (!output_file_name) {
            perror("malloc");
            return 1;
        }
        snprintf(output_file_name, base_len + 3, "%.*s.s", base_len, source_path);
    }
    else { // argc == 4
        // Support both argument orders
        if (strcmp(argv[1], "-o") == 0) {
            // Format: -o output.s source.zf
            source_path = argv[3];
            output_file_name = strdup(argv[2]);
        }
        else if (strcmp(argv[2], "-o") == 0) {
            // Format: source.zf -o output.s
            source_path = argv[1];
            output_file_name = strdup(argv[3]);
        }
        else {
            printf("Error: invalid argument order.\n");
            printf("Usage: %s [-o output.s] source.zf  OR  %s source.zf [-o output.s]\n",
                   argv[0], argv[0]);
            return 1;
        }

        if (!output_file_name) {
            perror("strdup");
            return 1;
        }
    }


    int src_len = strlen(source_path);
    if (src_len < 3 || strcmp(source_path + src_len - 3, ".zf") != 0) {
        printf("Error: input file must have .zf extension\n");
        free(output_file_name);
        return 1;
    }


    int out_len = strlen(output_file_name);
    if (out_len < 2 || strcmp(output_file_name + out_len - 2, ".s") != 0) {
        printf("Error: output file must have .s extension\n");
        free(output_file_name);
        return 1;
    }


    Arena reader_arena, parser_arena, semantic_analyzer_arena, ir_arena;
    
    arena_init(&reader_arena);
    arena_init(&parser_arena);
    arena_init(&semantic_analyzer_arena);
    arena_init(&ir_arena);


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
        arena_free(&reader_arena);
        arena_free(&parser_arena);
        arena_free(&semantic_analyzer_arena);
        arena_free(&ir_arena);
        return 1;
    }

    if(!analyze_program(ast, &ctx))
    {
        printf(stderr, "Semantic analysis failed\n");
        arena_free(&reader_arena);
        arena_free(&parser_arena);
        arena_free(&semantic_analyzer_arena);
        arena_free(&ir_arena);
        return 1;
    }
    
    
    IRContext irc = {0};
    ir_context_init(&irc, &ir_arena, ast, &ctx);

    ir_generate(&irc);
    
    printf("Compiling %s → %s\n", source_path, output_file_name);
    arm64_generate(&irc, output_file_name);
    printf("Done\n");


    
    arena_free(&reader_arena);
    arena_free(&parser_arena);
    arena_free(&semantic_analyzer_arena);

    ir_vec_free(&irc.instructions_vector);

    arena_free(&ir_arena);

    free(output_file_name);

    return 0;
}
