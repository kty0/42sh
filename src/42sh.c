#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_print.h"
#include "domain_expansion/domain_expansion.h"
#include "eval/eval.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "quarantedeuxsh.h"

#define HASH_MAP_SIZE 42

static int n_arg = 0;

/* Sets up the # variable */
static void set_argc_var(struct hash_map *hash_map, enum source source)
{
    char *value = calloc(12, sizeof(char));

    char *key = calloc(2, sizeof(char));
    key[0] = '#';

    switch (source)
    {
    case ARGV:
        if (n_arg <= 3)
        {
            sprintf(value, "%d", 0);
        }
        else
        {
            sprintf(value, "%d", n_arg - 4);
        }
        hash_map_insert_op(hash_map, key, value);
        break;
    case SCRIPT:
        sprintf(value, "%d", n_arg - 2);
        hash_map_insert_op(hash_map, key, value);
        break;
    case STDIN:
        sprintf(value, "%d", 0);
        hash_map_insert_op(hash_map, key, value);
        break;
    default:
        break;
    }
}

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

    int res = 0;

    struct hash_map *hash_map = hash_map_init(HASH_MAP_SIZE);

    set_argc_var(hash_map, source);

    struct token tok = lexer_peek(lexer);

    while (tok.type != TOKEN_EOF && tok.type != TOKEN_ERROR)
    {
        if (parse(&ast, lexer) == P_KO)
        {
            lexer_free(lexer);
            ast_free(ast);
            hash_map_free(hash_map);
            errx(2, "mmh no gud, parsing failed somehow");
        }

        if (getenv("PRETTY"))
        {
            ast_print(ast);
            puts("");
        }

        res = eval(ast, hash_map);

        ast_free(ast);

        tok = lexer_peek(lexer);
    }

    hash_map_free(hash_map);

    if (source != STDIN)
    {
        fclose(stream);
    }
    lexer_free(lexer);

    return res;
}

int main(int argc, char *argv[])
{
    n_arg = argc;

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
