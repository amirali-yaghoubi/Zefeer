#include <stdio.h>
#include "common/arena.h"
#include "front/lexer.h"



const char* source_path = "test/lexer_test_source.txt";


char* read_file(Arena* a, const char* path);

//============
int main()
{
    Arena a = {0};
    arena_init(&a);

    char* source = read_file(&a, source_path);

    printf("Source code:\n");
    printf("______________________________\n");
    printf("%s\n", source);
    printf("______________________________\n\n\n");
    printf("Lexer:\n");
    printf("______________________________\n");

    Lexer l = {0};
    lexer_init(&l, source_path, source);

    printf("%-4s %-20s %-15s %-10s %-5s\n",
           "No", "Type", "Text", "Value", "Line");
    printf("---------------------------------------------------------------\n");

    int i = 0;
    Token tok = {0};

    while (tok.type != TOK_EOF)
    {
        tok = get_next_token(&l);

        printf("%-4d %-20s %-15s ",
               i++,
               token_type_to_str(tok.type),
               token_to_str(&a, tok.start, tok.length));

        if (tok.type == TOK_NUMBER)
            printf("%-10ld ", tok.value.int_value);
        else
            printf("%-10s ", "UNDEF");

        printf("%-5d\n", tok.line);
    }

    return 0;
}
//============

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

    size_t read = fread(buffer, 1, size, file);
    if (read != size)
    {
        fclose(file);
        fprintf(stderr, "Error reading file\n");
        return NULL;
    }

    buffer[size] = '\0';
    fclose(file);
    return buffer;
}
