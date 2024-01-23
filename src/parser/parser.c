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
static enum parser_status parse_rule_while(struct ast **res,
                                           struct lexer *lexer);
static enum parser_status parse_rule_until(struct ast **res,
                                           struct lexer *lexer);

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
        fprintf(stderr, "mmh no gud, parsing failed somehow\n");
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
    fprintf(stderr, "mmh no gud, parsing failed somehow\n");
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

        *res = list;

        return P_OK;
    }

    return P_KO;
}

static enum parser_status parse_and_or(struct ast **res, struct lexer *lexer)
{
    /* Parsing the first pipeline */

    if (parse_pipeline(res, lexer) != P_OK)
    {
        return P_KO;
    }

    struct ast *node;

    /* Checking if there is an operator */

    struct token tok = lexer_peek_free(lexer);

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

        tok = lexer_peek_free(lexer);
    }

    *res = node;

    return P_OK;
}

static enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer)
{
    /* Checking if there is a TOKEN_NOT to begin with for later */

    struct token tok = lexer_peek_free(lexer);
    int is_not = tok.type == TOKEN_NOT;

    if (is_not)
    {
        tok = lexer_pop_free(lexer);
    }

    if (parse_command(res, lexer) != P_OK)
    {
        return P_KO;
    }

    struct ast *node;

    /* Checking if there is a pipe */

    tok = lexer_peek_free(lexer);

    if (tok.type != TOKEN_PIPE)
    {
        node = *res;
    }
    else
    {
        node = ast_new(AST_PIPE);

        struct ast_pipe *ast_pipe = &node->data.ast_pipe;

        ast_pipe->command = *res;

        while (tok.type == TOKEN_PIPE)
        {
            tok = lexer_pop_free(lexer);

            if (parse_command(res, lexer) != P_OK)
            {
                ast_free(node);

                return P_KO;
            }

            struct ast *new_ast = ast_new(AST_PIPE);

            struct ast_pipe *new_pipe = &new_ast->data.ast_pipe;

            new_pipe->command = *res;

            ast_pipe->child = new_ast;

            ast_pipe = new_pipe;

            tok = lexer_peek_free(lexer);
        }
    }

    /* Adding the not if there was one */

    if (is_not)
    {
        struct ast *node_not = ast_new(AST_NOT);

        struct ast_not *ast_not = &node_not->data.ast_not;

        ast_not->child = node;

        *res = node_not;
    }
    else
    {
        *res = node;
    }

    return P_OK;
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
    return parse_rule_if(res, lexer) == P_OK
            || parse_rule_while(res, lexer) == P_OK
            || parse_rule_until(res, lexer) == P_OK
        ? P_OK
        : P_KO;
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

static enum parser_status parse_rule_while(struct ast **res,
                                           struct lexer *lexer)
{
    struct token tok = lexer_peek_free(lexer);

    if (tok.type != TOKEN_WHILE)
    {
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    if (parse_compound_list(res, lexer) != P_OK)
    {
        return P_KO;
    }

    struct ast *ast = ast_new(AST_WHILE);

    struct ast_while *ast_while = &ast->data.ast_while;

    ast_while->condition = *res;

    tok = lexer_peek_free(lexer);

    if (tok.type != TOKEN_DO)
    {
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    if (parse_compound_list(res, lexer) != P_OK)
    {
        return P_KO;
    }

    ast_while->body = *res;

    tok = lexer_peek_free(lexer);

    if (tok.type != TOKEN_DONE)
    {
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    *res = ast;

    return P_OK;
}

static enum parser_status parse_rule_until(struct ast **res,
                                           struct lexer *lexer)
{
    struct token tok = lexer_peek_free(lexer);

    if (tok.type != TOKEN_UNTIL)
    {
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    if (parse_compound_list(res, lexer) != P_OK)
    {
        return P_KO;
    }

    struct ast *ast = ast_new(AST_UNTIL);

    struct ast_until *ast_until = &ast->data.ast_until;

    ast_until->condition = *res;

    tok = lexer_peek_free(lexer);

    if (tok.type != TOKEN_DO)
    {
        ast_free(ast);
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    if (parse_compound_list(res, lexer) != P_OK)
    {
        ast_free(ast);
        return P_KO;
    }

    ast_until->body = *res;

    tok = lexer_peek_free(lexer);

    if (tok.type != TOKEN_DONE)
    {
        ast_free(ast);
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    *res = ast;

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
