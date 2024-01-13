#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "eval/eval.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "quarantedeuxsh.h"

static char *get_input(enum source source, char *file);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        char *input = get_input(STDIN, NULL);
        if (input == NULL)
        {
            return 1;
        }

        struct lexer *lexer = lexer_new(input);
        struct ast *ast;

        free(input);

        if (parse(&ast, lexer) == P_KO)
        {
            lexer_free(lexer);
            errx(1, "42sh: Syntax error");
        }

        lexer_free(lexer);

        if (getenv("PRETTY"))
        {
            ast_print(ast);
            puts("");
        }

        int res = eval(ast);

        ast_free(ast);

        return res;
    }

    if (argv[1][0] == '-')
    {
        if (strcmp(argv[1], "-c") == 0)
        {
            struct lexer *lexer = lexer_new(argv[2]);
            struct ast *ast;

            if (parse(&ast, lexer) == P_KO)
            {
                lexer_free(lexer);
                errx(1, "42sh: Syntax error");
            }

            lexer_free(lexer);

            if (getenv("PRETTY"))
            {
                ast_print(ast);
                puts("");
            }

            int res = eval(ast);

            ast_free(ast);

            return res;
        }
        else
        {
            errx(1, "42sh: Usage: ./42sh [OPTIONS] [SCRIPT] [ARGUMENTS...]");
        }
    }
    else
    {
        char *input = get_input(SCRIPT, argv[1]);
        if (input == NULL)
        {
            return 1;
        }

        struct lexer *lexer = lexer_new(input);
        struct ast *ast;

        free(input);

        if (parse(&ast, lexer) == P_KO)
        {
            lexer_free(lexer);
            errx(1, "42sh: Syntax error");
        }

        lexer_free(lexer);

        if (getenv("PRETTY"))
        {
            ast_print(ast);
            puts("");
        }

        int res = eval(ast);

        ast_free(ast);

        return res;
    }

    return 1;
}

static char *get_input(enum source source, char *file)
{
    FILE *fd = source == SCRIPT ? fopen(file, "r") : stdin;
    if (fd == NULL)
    {
        return NULL;
    }

    char *input = malloc(BUFFER_SIZE);
    if (input == NULL)
    {
        fclose(fd);

        return NULL;
    }

    int iter = 0;

    while (fread(input + iter * BUFFER_SIZE, sizeof(char), BUFFER_SIZE, fd)
           == BUFFER_SIZE)
    {
        char *temp = realloc(input, BUFFER_SIZE * (++iter + 1));
        if (temp == NULL)
        {
            free(input);
            fclose(fd);

            return NULL;
        }
        input = temp;
    }

    fclose(fd);

    return input;
}
