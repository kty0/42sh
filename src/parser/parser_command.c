#include "parser_command.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "../ast/ast_new.h"
#include "../lexer/lexer.h"
#include "parser.h"
#include "parser_if.h"
#include "parser_io.h"
#include "parser_loop.h"
#include "parser_misc.h"

enum parser_status parse_command(struct ast **res, struct lexer *lexer)
{
    return parse_simple_command(res, lexer) == P_OK
            || parse_shell_command(res, lexer) == P_OK
        ? P_OK
        : P_KO;
}

enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer)
{
    return parse_rule_if(res, lexer) == P_OK
            || parse_rule_while(res, lexer) == P_OK
            || parse_rule_until(res, lexer) == P_OK
        ? P_OK
        : P_KO;
}

enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type == TOKEN_WORD)
    {
        tok = lexer_pop(lexer);

        struct ast *node = ast_new(AST_COMMAND);

        if (ast_cmd_push(node, tok.value))
        {
            ast_free(node);
            return P_KO;
        }

        *res = node;

        while (parse_element(res, lexer) == P_OK)
        {
            continue;
        }

        return P_OK;
    }

    return P_KO;
}
