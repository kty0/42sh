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
    return parse_shell_command(res, lexer) == P_OK
            || (parse_simple_command(res, lexer) == P_OK && *res != NULL)
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

/* Unintuitively parsing a not so simple command:
 *
 * architecture of the command shall be a degenerated tree with redirections
 * on the right and a single command as the sole leaf on the left
 *
 *     R ───────╮
 *              │
 *              R ───────╮
 *                       │
 *                       R ───────╮
 *                                │
 *                       ╭─────── R
 *                       │
 *                       C
 */
enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer)
{
    *res = NULL;

    /* Creating the result node being the root and the node node being
     * the last child */

    struct token tok = lexer_peek(lexer);
    struct ast *result = NULL;
    struct ast *node = NULL;

    /* Parsing early redirections */

    while (parse_prefix(res, lexer) != P_KO)
    {
        if (!node)
        {
            node = *res;
            result = *res;
        }
        else
        {
            struct ast_redir *ast_redir = &node->data.ast_redir;
            ast_redir->right = *res;
            node = *res;
        }
    }

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD)
    {
        return P_OK;
    }

    /* Creating the command and placing it at the right place in the tree */

    tok = lexer_pop(lexer);

    struct ast *ast_cmd = ast_new(AST_COMMAND);

    ast_cmd_push(ast_cmd, tok.value);

    if (!node)
    {
        node = ast_cmd;
        result = ast_cmd;
    }
    else
    {
        struct ast_redir *ast_redir = &node->data.ast_redir;
        ast_redir->left = ast_cmd;
    }

    /* Parsing the following elements */

    tok = lexer_peek(lexer);

    while (parse_element(res, lexer) != P_KO)
    {
        /* If it's a word it goes in the command */

        if ((*res)->type == AST_WORD)
        {
            struct ast_word *word = &(*res)->data.ast_word;
            ast_cmd_push(ast_cmd, word->arg);
            free(*res);
        }

        /* Else it is appended to the tree and the command leaf is moved */

        else
        {
            struct ast_redir *ast_redir = &(*res)->data.ast_redir;

            if (node->type == AST_COMMAND)
            {
                ast_redir->left = node;
                node = *res;
                result = *res;
            }
            else
            {
                struct ast_redir *old_node = &node->data.ast_redir;

                ast_redir->left = old_node->left;
                old_node->right = *res;
                old_node->left = NULL;
                node = *res;
            }
        }
    }

    *res = result;

    return P_OK;
}
