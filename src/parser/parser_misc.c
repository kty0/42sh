#include "parser_misc.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "../ast/ast_new.h"
#include "../lexer/lexer.h"
#include "parser.h"
#include "parser_command.h"
#include "parser_if.h"
#include "parser_io.h"
#include "parser_loop.h"

enum parser_status parse_list(struct ast **res, struct lexer *lexer)
{
    if (parse_and_or(res, lexer) == P_OK)
    {
        struct ast *list = ast_new(AST_LIST);
        ast_list_push(list, *res);

        struct token tok = lexer_peek(lexer);

        while (tok.type == TOKEN_SEMICOLON)
        {
            tok = lexer_pop_free(lexer);

            if (parse_and_or(res, lexer) != P_OK)
            {
                break;
            }

            ast_list_push(list, *res);

            tok = lexer_peek(lexer);
        }

        *res = list;

        return P_OK;
    }

    return P_KO;
}

enum parser_status parse_and_or(struct ast **res, struct lexer *lexer)
{
    /* Parsing the first pipeline */

    if (parse_pipeline(res, lexer) != P_OK)
    {
        return P_KO;
    }

    struct ast *node;

    /* Checking if there is an operator */

    struct token tok = lexer_peek(lexer);

    if (tok.type != TOKEN_AND_IF && tok.type != TOKEN_OR_IF)
    {
        return P_OK;
    }

    node = ast_new(AST_OPERATOR);

    struct ast_ope *ast_ope = &node->data.ast_ope;

    ast_ope->type = tok.type == TOKEN_AND_IF ? AND : OR;
    ast_ope->left = *res;

    while (tok.type == TOKEN_AND_IF || tok.type == TOKEN_OR_IF)
    {
        tok = lexer_pop_free(lexer);

        if (parse_command(res, lexer) != P_OK)
        {
            ast_free(node);

            return P_KO;
        }

        struct ast *new_ast = ast_new(AST_OPERATOR);

        struct ast_ope *new_ope = &new_ast->data.ast_ope;

        new_ope->left = *res;

        ast_ope->right = new_ast;

        ast_ope = new_ope;

        tok = lexer_peek(lexer);
    }

    *res = node;

    return P_OK;
}

enum parser_status parse_compound_list(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    while (tok.type == TOKEN_NEWLINE)
    {
        tok = lexer_pop_free(lexer);

        tok = lexer_peek(lexer);
    }

    if (parse_and_or(res, lexer) == P_OK)
    {
        struct ast *list = ast_new(AST_LIST);

        ast_list_push(list, *res);

        struct token tok = lexer_peek(lexer);

        while (tok.type == TOKEN_SEMICOLON || tok.type == TOKEN_NEWLINE)
        {
            tok = lexer_pop_free(lexer);

            tok = lexer_peek(lexer);

            while (tok.type == TOKEN_NEWLINE)
            {
                tok = lexer_pop_free(lexer);

                tok = lexer_peek(lexer);
            }

            if (parse_and_or(res, lexer) == P_KO)
            {
                break;
            }

            ast_list_push(list, *res);

            tok = lexer_peek(lexer);
        }

        tok = lexer_peek(lexer);

        while (tok.type == TOKEN_NEWLINE)
        {
            tok = lexer_pop_free(lexer);

            tok = lexer_peek(lexer);
        }

        *res = list;

        return P_OK;
    }

    return P_KO;
}

enum parser_status parse_element(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type == TOKEN_WORD)
    {
        tok = lexer_pop(lexer);

        ast_cmd_push(*res, tok.value);

        return P_OK;
    }

    return P_KO;
}
