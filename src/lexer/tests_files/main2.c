#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../lexer.h"
#include "../token.h"

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
                [TOKEN_IN] = "in",
                [TOKEN_AND_IF] = "&&",
                [TOKEN_OR_IF] = "||" };

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;
    FILE *fd = fopen(argv[1], "r");

    struct stat sb;
    if (stat(argv[1], &sb) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    struct lexer *lexer = lexer_new(fd);
    struct token token = lexer->current_tok;

    while (token.type != TOKEN_ERROR && token.type != TOKEN_EOF)
    {
        if (token.type == TOKEN_WORD)
            printf("%s:\t%s\n", tab[token.type], token.value);
        else
            printf("%s\n", tab[token.type]);

        free_token(token);
        lexer_pop(lexer);
        token = lexer->current_tok;
    }
    free_token(token);
    if (token.type == TOKEN_ERROR)
        printf("ALED\n");
    if (token.type == TOKEN_EOF)
        printf("eof\n");

    lexer_free(lexer);

    return 0;
}
