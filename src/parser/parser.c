#include "parser.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "../ast/ast_new.h"
#include "../lexer/lexer.h"
#include "parser_command.h"
#include "parser_if.h"
#include "parser_io.h"
#include "parser_loop.h"
#include "parser_misc.h"

enum parser_status parse(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type == TOKEN_NEWLINE || tok.type == TOKEN_EOF)
    {
        if (tok.type == TOKEN_NEWLINE)
        {
            lexer_pop_free(lexer);
        }

        *res = NULL;
        return P_OK;
    }

    if (parse_list(res, lexer) != P_OK)
    {
        ast_free(*res);
        fprintf(stderr, "mmh no gud, parsing failed somehow\n");
        return P_KO;
    }

    tok = lexer_peek(lexer);

    if (tok.type == TOKEN_NEWLINE || tok.type == TOKEN_EOF)
    {
        if (tok.type == TOKEN_NEWLINE)
        {
            lexer_pop_free(lexer);
        }

        return P_OK;
    }

    ast_free(*res);
    fprintf(stderr, "mmh no gud, parsing failed somehow\n");
    return P_KO;
}
