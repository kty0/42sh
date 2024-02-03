#include "parser_loop.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../ast/ast_new.h"
#include "../lexer/lexer.h"
#include "parser.h"
#include "parser_command.h"
#include "parser_if.h"
#include "parser_io.h"
#include "parser_misc.h"

enum parser_status parse_rule_while(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "while"))
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

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "do"))
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

    ast_while->body = *res;

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "done"))
    {
        ast_free(ast);
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    *res = ast;

    return P_OK;
}

enum parser_status parse_rule_until(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "until"))
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

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "do"))
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

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "done"))
    {
        ast_free(ast);
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    *res = ast;

    return P_OK;
}

static enum parser_status parse_in(struct lexer *lexer, struct ast *ast)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type != TOKEN_SEMICOLON)
    {
        while (tok.type == TOKEN_NEWLINE)
        {
            tok = lexer_pop_free(lexer);

            tok = lexer_peek(lexer);
        }

        if (tok.type == TOKEN_WORD && !strcmp(tok.value, "in"))
        {
            tok = lexer_pop_free(lexer);
            tok = lexer_peek(lexer);
            while (tok.type == TOKEN_WORD)
            {
                tok = lexer_pop(lexer);

                ast_for_push(ast, tok.value);

                tok = lexer_peek(lexer);
            }

            if (tok.type != TOKEN_SEMICOLON && tok.type != TOKEN_NEWLINE)
            {
                ast_free(ast);

                return P_KO;
            }
        }
    }
    if (tok.type != TOKEN_WORD || strcmp(tok.value, "do"))
    {
        tok = lexer_pop_free(lexer);
    }
    return P_OK;
}

enum parser_status parse_rule_for(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "for"))
    {
        return P_KO;
    }

    struct ast *ast = ast_new(AST_FOR);

    tok = lexer_pop_free(lexer);

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD)
    {
        ast_free(ast);

        return P_KO;
    }

    tok = lexer_pop(lexer);

    struct ast_for *ast_for = &ast->data.ast_for;

    ast_for->var = tok.value;

    tok = lexer_peek(lexer);

    if (parse_in(lexer, ast) != P_OK)
    {
        return P_KO;
    }

    tok = lexer_peek(lexer);
    while (tok.type == TOKEN_NEWLINE)
    {
        tok = lexer_pop_free(lexer);

        tok = lexer_peek(lexer);
    }

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "do"))
    {
        return P_KO;
    }
    tok = lexer_pop_free(lexer);
    tok = lexer_peek(lexer);

    if (parse_compound_list(res, lexer) != P_OK)
    {
        ast_free(ast);

        return P_KO;
    }

    ast_for->body = *res;

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "done"))
    {
        ast_free(ast);

        return P_KO;
    }
    tok = lexer_pop_free(lexer);
    tok = lexer_peek(lexer);
    *res = ast;

    tok = lexer_pop_free(lexer);
    return P_OK;
}
