#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
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
                [TOKEN_WORD] = "words",
                [TOKEN_ERROR] = "error",
                [TOKEN_EOF] = "eof" };

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;
    int fd = open(argv[1], O_RDONLY);
    char buffer[4096];

    struct stat sb;
    if (stat(argv[1], &sb) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    read(fd, buffer, sb.st_size);
    buffer[sb.st_size] = '\0';

    struct lexer *lexer = lexer_new(buffer);
    struct token token = lexer_pop(lexer);

    while (token.type != TOKEN_ERROR && token.type != TOKEN_EOF)
    {
        if (token.type == TOKEN_WORD)
            printf("%s:\t%s\n", tab[token.type], token.value);
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
