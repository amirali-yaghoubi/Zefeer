/*
 Test the lexer
 */


#include <stdio.h>
#include <stdlib.h>
#include "common/arena.h"
#include "front/lexer.h"


char* read_file(Arena* a, const char* path);

int main()
{
    Arena a;
    arena_init(&a);
    const char* source_path = "src/test_source.txt";

    char* source = read_file(&a, source_path);
    if (!source)
    {
        fprintf(stderr, "Failed to read %s\n", source_path);
        return 1;
    }

    Lexer lexer = {0};
    lexer_init(&lexer, source);

    printf("source code:\n\n");
    printf("_______________\n");
    printf("%s\n", source);
    printf("_______________\n\n");
    printf("Lexer output:\n\n");
    printf("----------------\n\n");
    printf("TOKEN              LEN     VALUE     LINE  START\n\n");

    while (1)
    {
    Token token = get_next_token(&lexer);

    if (token.type == TOK_EOF) break;
    
    printf("%-18s %-7d %-9ld %-5d %p\n",
       TokenTypeStr((TokenType)token.type),
       token.length,
       token.value.int_value,
       token.line,
       token.start
    );

    }
    printf("\n----------------\n\n");

    arena_free(&a);

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
