#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "lexer.h"
#include "token.h"

char *tab[] = { [TOKEN_IF] = "if",
                [TOKEN_THEN] = "then",
                [TOKEN_ELIF] = "elif",
                [TOKEN_ELSE] = "else",
                [TOKEN_FI] = "fi",
                [TOKEN_SEMICOLON] = ";",
                [TOKEN_NEWLINE] = "bah new line hein",
                [TOKEN_WORDS] = "words",
                [TOKEN_ERROR] = "error",
                [TOKEN_EOF] = "eof" };

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;
    int fd = open(argv[1], O_RDONLY);
    char buffer[4096];
    read(fd, buffer, 4096);

    struct lexer *lexer = lexer_new(buffer);
    struct token token = lexer_pop(lexer);

    while (token.type != TOKEN_ERROR && token.type != TOKEN_EOF)
    {
        if (token.type == TOKEN_WORDS)
            printf("%s:\t%s\n", tab[token.type], token.str);
        else
            printf("%s\n", tab[token.type]);

        free_token(token);
        token = lexer_pop(lexer);
    }
    if (token.type == TOKEN_ERROR)
        printf("ALED\n");
    free_token(token);
    if (token.type == TOKEN_EOF)
        printf("eof\n");

    lexer_free(lexer);

    return 0;
}
