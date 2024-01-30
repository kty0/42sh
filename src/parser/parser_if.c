#include "parser_if.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "../ast/ast_new.h"
#include "../lexer/lexer.h"
#include "parser.h"
#include "parser_command.h"
#include "parser_io.h"
#include "parser_loop.h"
#include "parser_misc.h"

enum parser_status parse_rule_if(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type != TOKEN_IF)
    {
        return P_KO;
    }

    lexer_pop_free(lexer);

    struct ast *node_if = ast_new(AST_IF);

    if (parse_compound_list(res, lexer) == P_KO)
    {
        ast_free(node_if);
        return P_KO;
    }

    struct ast_if *ast_if = &node_if->data.ast_if;

    ast_if->condition = *res;

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_THEN)
    {
        ast_free(node_if);
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    if (parse_compound_list(res, lexer) == P_KO)
    {
        ast_free(node_if);
        return P_KO;
    }

    ast_if->then_body = *res;

    tok = lexer_peek(lexer);

    *res = node_if;

    if (tok.type == TOKEN_FI)
    {
        tok = lexer_pop_free(lexer);
        return P_OK;
    }

    if (parse_else_clause(res, lexer) != P_OK)
    {
        ast_free(node_if);
        return P_KO;
    }

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_FI)
    {
        ast_free(node_if);
        return P_KO;
    }

    lexer_pop_free(lexer);

    *res = node_if;

    return P_OK;
}

enum parser_status parse_else_clause(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    struct ast *node_if = *res;

    if (tok.type == TOKEN_ELSE)
    {
        tok = lexer_pop_free(lexer);

        if (parse_compound_list(res, lexer) == P_OK)
        {
            struct ast_if *ast_if = &node_if->data.ast_if;

            ast_if->else_body = *res;

            return P_OK;
        }
        else
        {
            return P_KO;
        }
    }
    else if (tok.type == TOKEN_ELIF)
    {
        tok = lexer_pop_free(lexer);

        if (parse_compound_list(res, lexer) != P_OK)
        {
            return P_KO;
        }

        struct ast *ast = ast_new(AST_IF);

        struct ast_if *prev_if = &node_if->data.ast_if;

        prev_if->else_body = ast;

        struct ast_if *ast_if = &ast->data.ast_if;

        ast_if->condition = *res;

        tok = lexer_peek(lexer);

        if (tok.type != TOKEN_THEN)
        {
            return P_KO;
        }

        tok = lexer_pop_free(lexer);

        if (parse_compound_list(res, lexer) != P_OK)
        {
            return P_KO;
        }

        ast_if->then_body = *res;

        tok = lexer_peek(lexer);

        if (tok.type == TOKEN_ELSE || tok.type == TOKEN_ELIF)
        {
            *res = ast;

            if (parse_else_clause(res, lexer) != P_OK)
            {
                return P_KO;
            }
        }

        return P_OK;
    }

    return P_OK;
}
