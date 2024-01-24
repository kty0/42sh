#include "parser_loop.h"

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
#include "parser_misc.h"

enum parser_status parse_rule_while(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

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

    tok = lexer_peek(lexer);

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

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_DONE)
    {
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    *res = ast;

    return P_OK;
}

enum parser_status parse_rule_until(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

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

    tok = lexer_peek(lexer);

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

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_DONE)
    {
        ast_free(ast);
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    *res = ast;

    return P_OK;
}
