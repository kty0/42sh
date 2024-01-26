#include "parser_io.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "../ast/ast_new.h"
#include "../lexer/lexer.h"
#include "parser.h"
#include "parser_command.h"
#include "parser_if.h"
#include "parser_loop.h"
#include "parser_misc.h"

enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer)
{
    /* Checking if there is a TOKEN_NOT to begin with for later */

    struct token tok = lexer_peek(lexer);
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

    tok = lexer_peek(lexer);

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

            tok = lexer_peek(lexer);
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

enum parser_status parse_redirection(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    int io_nb = -1;

    if (tok.type == TOKEN_IONUMBER)
    {
        io_nb = atoi(tok.value);
        lexer_pop_free(lexer);
    }

    tok = lexer_peek(lexer);

    if (tok.type == TOKEN_LESS || tok.type == TOKEN_GREAT
        || tok.type == TOKEN_LESSAND || tok.type == TOKEN_GREATAND
        || tok.type == TOKEN_LESSGREAT || tok.type == TOKEN_DGREAT
        || tok.type == TOKEN_CLOBBER)
    {
        tok = lexer_pop_free(lexer);

        struct ast *node = ast_new_redir(tok.type);
        struct ast_redir *ast_redir = &node->data.ast_redir;

        if (io_nb != -1)
        {
            ast_redir->fd = io_nb;
        }
        else
        {
            if (tok.type == TOKEN_GREAT || tok.type == TOKEN_CLOBBER
                || tok.type == TOKEN_DGREAT || tok.type == TOKEN_GREATAND)
            {
                ast_redir->fd = 1; // stdout
            }
            else if (tok.type == TOKEN_LESS || tok.type == TOKEN_LESSGREAT
                     || tok.type == TOKEN_LESSAND)
            {
                ast_redir->fd = 0; // stdin
            }
        }

        tok = lexer_peek(lexer);

        if (tok.type != TOKEN_WORD)
        {
            return P_KO;
        }

        tok = lexer_pop(lexer);
        ast_redir->file = tok.value;

        *res = node;

        return P_OK;
    }

    return P_KO;
}

enum parser_status parse_prefix(struct ast **res, struct lexer *lexer)
{
    return parse_redirection(res, lexer);
}
