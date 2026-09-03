#include <stdio.h>
#include "common/arena.h"
#include "common/file.h"
#include "front/lexer.h"



const char* source_path = "test/lexer_test_source.zf";



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
