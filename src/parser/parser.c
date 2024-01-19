#include "parser.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "../ast/ast_new.h"
#include "../lexer/lexer.h"

static enum parser_status parse_element(struct ast **res, struct lexer *lexer);
static enum parser_status parse_simple_command(struct ast **res,
                                               struct lexer *lexer);
static enum parser_status parse_else_clause(struct ast **res,
                                            struct lexer *lexer);
static enum parser_status parse_compound_list(struct ast **res,
                                              struct lexer *lexer);
static enum parser_status parse_rule_if(struct ast **res, struct lexer *lexer);
static enum parser_status parse_shell_command(struct ast **res,
                                              struct lexer *lexer);
static enum parser_status parse_command(struct ast **res, struct lexer *lexer);
static enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer);
static enum parser_status parse_and_or(struct ast **res, struct lexer *lexer);
static enum parser_status parse_list(struct ast **res, struct lexer *lexer);

enum parser_status parse(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek_free(lexer);

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
        fprintf(stderr, "Mmh no gud\n");
        return P_KO;
    }

    tok = lexer_peek_free(lexer);

    if (tok.type == TOKEN_NEWLINE || tok.type == TOKEN_EOF)
    {
        if (tok.type == TOKEN_NEWLINE)
        {
            lexer_pop_free(lexer);
        }

        return P_OK;
    }

    ast_free(*res);
    fprintf(stderr, "Mmh no gud\n");
    return P_KO;
}

static enum parser_status parse_list(struct ast **res, struct lexer *lexer)
{
    if (parse_and_or(res, lexer) == P_OK)
    {
        struct ast *list = ast_new(AST_LIST);
        ast_list_push(list, *res);

        struct token tok = lexer_peek_free(lexer);

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

        free_token(tok);

        *res = list;

        return P_OK;
    }

    return P_KO;
}

static enum parser_status parse_and_or(struct ast **res, struct lexer *lexer)
{
    return parse_pipeline(res, lexer);
}

static enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek_free(lexer);

    if (tok.type == TOKEN_NOT)
    {
        tok = lexer_pop_free(lexer);

        struct ast *node_not = ast_new(AST_NOT);

        if (parse_command(res, lexer) != P_OK)
        {
            return P_KO;
        }

        struct ast_not *ast_not = &node_not->data.ast_not;

        ast_not->child = *res;

        *res = node_not;

        return P_OK;
    }

    return parse_command(res, lexer);
}

static enum parser_status parse_command(struct ast **res, struct lexer *lexer)
{
    return parse_simple_command(res, lexer) == P_OK
            || parse_shell_command(res, lexer) == P_OK
        ? P_OK
        : P_KO;
}

static enum parser_status parse_shell_command(struct ast **res,
                                              struct lexer *lexer)
{
    return parse_rule_if(res, lexer);
}

static enum parser_status parse_rule_if(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek_free(lexer);

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

    tok = lexer_peek_free(lexer);

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

    tok = lexer_peek_free(lexer);

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

    tok = lexer_peek_free(lexer);

    if (tok.type != TOKEN_FI)
    {
        return P_KO;
    }

    lexer_pop_free(lexer);

    *res = node_if;

    return P_OK;
}

static enum parser_status parse_compound_list(struct ast **res,
                                              struct lexer *lexer)
{
    struct token tok = lexer_peek_free(lexer);

    while (tok.type == TOKEN_NEWLINE)
    {
        tok = lexer_pop_free(lexer);

        tok = lexer_peek_free(lexer);
    }

    if (parse_and_or(res, lexer) == P_OK)
    {
        struct ast *list = ast_new(AST_LIST);

        ast_list_push(list, *res);

        struct token tok = lexer_peek_free(lexer);

        while (tok.type == TOKEN_SEMICOLON || tok.type == TOKEN_NEWLINE)
        {
            tok = lexer_pop_free(lexer);

            tok = lexer_peek_free(lexer);

            while (tok.type == TOKEN_NEWLINE)
            {
                tok = lexer_pop_free(lexer);

                tok = lexer_peek_free(lexer);
            }

            if (parse_and_or(res, lexer) == P_KO)
            {
                break;
            }

            ast_list_push(list, *res);

            tok = lexer_peek_free(lexer);
        }

        tok = lexer_peek_free(lexer);

        while (tok.type == TOKEN_NEWLINE)
        {
            tok = lexer_pop_free(lexer);

            tok = lexer_peek_free(lexer);
        }

        *res = list;

        return P_OK;
    }

    return P_KO;
}

static enum parser_status parse_else_clause(struct ast **res,
                                            struct lexer *lexer)
{
    struct token tok = lexer_peek_free(lexer);

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

        tok = lexer_peek_free(lexer);

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

        tok = lexer_peek_free(lexer);

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

static enum parser_status parse_simple_command(struct ast **res,
                                               struct lexer *lexer)
{
    struct token tok = lexer_peek_free(lexer);

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

static enum parser_status parse_element(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek_free(lexer);

    if (tok.type == TOKEN_WORD)
    {
        tok = lexer_pop(lexer);

        ast_cmd_push(*res, tok.value);

        return P_OK;
    }

    return P_KO;
}
