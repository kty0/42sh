#include <stdio.h>

#include "lexer.h"
#include "token.h"

char *tab[] = { [TOKEN_IF] = "if",
                [TOKEN_THEN] = "then",
                [TOKEN_ELIF] = "elif",
                [TOKEN_ELSE] = "else",
                [TOKEN_FI] = "fi",
                [TOKEN_SEMICOLON] = ";",
                [TOKEN_NEWLINE] = "bah new line hein",
                [TOKEN_WORD] = "words",
                [TOKEN_ERROR] = "error",
                [TOKEN_EOF] = "eof",
                [TOKEN_NOT] = "!",
                [TOKEN_PIPE] = "|",
                [TOKEN_WHILE] = "while",
                [TOKEN_UNTIL] = "until",
                [TOKEN_DO] = "do",
                [TOKEN_DONE] = "done",
                [TOKEN_FOR] = "for",
                [TOKEN_IN] = "in" };

int main(int argc, char *argv[])
{
    if (argc == 1)
        return 1;

    struct lexer *lexer = lexer_new(argv[1]);
    struct token token = lexer_pop(lexer);

    while (token.type != TOKEN_NEWLINE && token.type != TOKEN_ERROR
           && token.type != TOKEN_EOF)
    {
        if (token.type == TOKEN_WORD)
            printf("%s : %s\n", tab[token.type], token.value);
        else
            printf("%s\n", tab[token.type]);

        free_token(token);
        token = lexer_pop(lexer);
    }
    if (token.type == TOKEN_ERROR)
        printf("ALED\n");
    if (token.type == TOKEN_NEWLINE)
        printf("newline\n");
    if (token.type == TOKEN_EOF)
        printf("eof\n");
    free_token(token);

    lexer_free(lexer);

    return 0;
}
