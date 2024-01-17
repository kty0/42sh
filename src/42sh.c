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
static int eval_input(char *input, enum source source);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        char *input = get_input(STDIN, NULL);
        if (input == NULL)
        {
            return 1;
        }
        int result = eval_input(input, STDIN);
        if (result == -1)
        {
            errx(1, "syntax error");
        }
        return result;
    }

    if (argv[1][0] == '-')
    {
        if (strcmp(argv[1], "-c") == 0)
        {
            int result = eval_input(argv[2], ARGV);
            if (result == -1)
            {
                errx(1, "syntax error");
            }
            return result;
        }
        else
        {
            errx(1, "usage: ./42sh [OPTIONS] [SCRIPT] [ARGUMENTS...]");
        }
    }
    else
    {
        char *input = get_input(SCRIPT, argv[1]);
        if (input == NULL)
        {
            return 1;
        }

        int result = eval_input(input, SCRIPT);
        if (result == -1)
        {
            errx(1, "syntax error");
        }
        return result;
    }

    return 1;
}

static int eval_input(char *input, enum source source)
{
    struct lexer *lexer = lexer_new(input);
    struct ast *ast;

    struct token tok = lexer_peek_free(lexer);
    int res = 0;

    while (tok.type != TOKEN_EOF)
    {
        if (parse(&ast, lexer) == P_KO)
        {
            lexer_free(lexer);
            return 1;
        }

        if (getenv("PRETTY"))
        {
            ast_print(ast);
            puts("");
        }

        res = eval(ast);

        ast_free(ast);

        tok = lexer_peek_free(lexer);
    }

    if (source != ARGV)
    {
        free(input);
    }
    lexer_free(lexer);

    return res;
}

static char *get_input(enum source source, char *file)
{
    FILE *fd = source == SCRIPT ? fopen(file, "r") : stdin;
    if (fd == NULL)
    {
        return NULL;
    }

    char *input = calloc(BUFFER_SIZE, sizeof(char));
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

        for (int i = BUFFER_SIZE * iter; i < BUFFER_SIZE * (iter + 1); i++)
        {
            input[i] = 0;
        }
    }

    fclose(fd);

    return input;
}
