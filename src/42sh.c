#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "domain_expansion/domain_expansion.h"
#include "eval/eval.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "quarantedeuxsh.h"
#include "domain_expansion/domain_expansion.h"

/**
 *  \brief  From a given source, loops between parsing an expr and evaluating it
 *  \fn     static int parse_eval(FILE *stream, enum source source);
 *  \param  stream The file descriptor of the source
 *  \param  source The source of the stream
 *  \return The return value of the last evaluation
 */
static int parse_eval(FILE *stream, enum source source)
{
    struct lexer *lexer = lexer_new(stream);
    struct ast *ast;

    struct token tok = lexer_peek(lexer);

    int res = 0;

    while (tok.type != TOKEN_EOF && tok.type != TOKEN_ERROR)
    {
        if (parse(&ast, lexer) == P_KO)
        {
            lexer_free(lexer);
            ast_free(ast);
            errx(2, "mmh no gud, parsing failed somehow");
        }

        if (getenv("PRETTY"))
        {
            ast_print(ast);
            puts("");
        }
        res = eval(ast);

        ast_free(ast);

        tok = lexer_peek(lexer);
    }

    if (source != STDIN)
    {
        fclose(stream);
    }
    lexer_free(lexer);

    return res;
}

int main(int argc, char *argv[])
{
    /* case when no argument is given: reading from stdin */

    if (argc == 1)
    {
        int result = parse_eval(stdin, STDIN);
        if (result == -1)
        {
            errx(2, "syntax error detected in parsing");
        }

        return result;
    }

    /* case when an argument is given: tests for -c and reads from string */

    if (argv[1][0] == '-')
    {
        if (strcmp(argv[1], "-c") == 0)
        {
            FILE *stream = fmemopen(argv[2], strlen(argv[2]), "r");
            if (stream == NULL)
            {
                errx(1, "failed to read from arguments");
            }

            int result = parse_eval(stream, ARGV);
            if (result == -1)
            {
                errx(2, "syntax error detected in parsing");
            }

            return result;
        }
        else
        {
            errx(127, "usage: ./42sh [OPTIONS] [SCRIPT] [ARGUMENTS...]");
        }
    }

    /* else reads from the first argument treated as a file */

    FILE *stream = fopen(argv[1], "r");
    if (stream == NULL)
    {
        errx(1, "failed to open the script file");
    }

    int result = parse_eval(stream, SCRIPT);
    if (result == -1)
    {
        errx(2, "syntax error detected in parsing");
    }

    return result;
}
