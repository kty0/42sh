#include "parser_misc.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../ast/ast_new.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "parser.h"
#include "parser_command.h"
#include "parser_if.h"
#include "parser_io.h"
#include "parser_loop.h"

static int is_assignment(char *value)
{
    for (int i = 0; value[i] != '\0'; i++)
    {
        if (value[i] == '=')
        {
            return 1;
        }
    }

    return 0;
}

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

        if (parse_pipeline(res, lexer) != P_OK)
        {
            ast_free(node);

            return P_KO;
        }

        struct ast *new_ast = ast_new(AST_OPERATOR);

        struct ast_ope *new_ope = &new_ast->data.ast_ope;

        new_ope->type = tok.type == TOKEN_AND_IF ? AND : OR;
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

    /* Either it could be a redirection which is parsed and returned */

    if (tok.type != TOKEN_WORD)
    {
        return parse_redirection(res, lexer);
    }

    /* Either it's a word which is also parsed and returned */

    tok = lexer_pop(lexer);

    struct ast *ast = ast_new(AST_WORD);

    struct ast_word *ast_word = &ast->data.ast_word;

    ast_word->arg = tok.value;
    *res = ast;

    return P_OK;
}

/* Upon finding an assignment splitting it around the '=' to create an
 * ast_assign node
 */
static enum parser_status parse_variable(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_pop(lexer);

    int len = strlen(tok.value);

    int i = 0;
    for (; tok.value[i] != '='; i++)
    {
        continue;
    }

    tok.value[i] = '\0';

    char *value = calloc(len - i, sizeof(char));
    strcpy(value, tok.value + i + 1);

    struct ast *ast = ast_new(AST_ASSIGNMENT);
    struct ast_assign *ast_assign = &ast->data.ast_assign;
    ast_assign->key = tok.value;
    ast_assign->value = value;

    *res = ast;

    return P_OK;
}

enum parser_status parse_prefix(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (parse_redirection(res, lexer) == P_OK)
    {
        return P_OK;
    }

    if (tok.type == TOKEN_WORD && is_assignment(tok.value))
    {
        return parse_variable(res, lexer);
    }

    return P_KO;
}
