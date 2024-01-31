#include "parser_command.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../ast/ast_new.h"
#include "../lexer/lexer.h"
#include "parser.h"
#include "parser_if.h"
#include "parser_io.h"
#include "parser_loop.h"
#include "parser_misc.h"

static int is_reserved(char *value)
{
    return !strcmp(value, "if") || !strcmp(value, "fi")
        || !strcmp(value, "then") || !strcmp(value, "do")
        || !strcmp(value, "while") || !strcmp(value, "until")
        || !strcmp(value, "done") || !strcmp(value, "else")
        || !strcmp(value, "elif");
}

enum parser_status parse_command(struct ast **res, struct lexer *lexer)
{
    return parse_shell_command(res, lexer) == P_OK
            || parse_funcdec(res, lexer) == P_OK
            || (parse_simple_command(res, lexer) == P_OK && *res != NULL)
        ? P_OK
        : P_KO;
}

enum parser_status parse_funcdec(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    struct token next_tok = lexer_super_peek(lexer);

    if (tok.type != TOKEN_WORD || next_tok.type != TOKEN_WORD || strcmp(next_tok.value, "("))
    {
        return P_KO;
    }


}

enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer)
{
    return parse_rule_if(res, lexer) == P_OK
            || parse_rule_while(res, lexer) == P_OK
            || parse_rule_until(res, lexer) == P_OK
        ? P_OK
        : P_KO;
}

static void parse_elements_loop(struct ast **res, struct lexer *lexer,
                                struct ast **ast_cmd, struct cmd_pack cmd_pack)
{
    struct ast **node = cmd_pack.node;
    struct ast **result = cmd_pack.result;

    while (parse_element(res, lexer) != P_KO)
    {
        /* If it's a word it goes in the command */

        if ((*res)->type == AST_WORD)
        {
            struct ast_word *word = &(*res)->data.ast_word;
            ast_cmd_push(*ast_cmd, word->arg, word->exp);
            free(*res);
        }

        /* Else it is appended to the tree and the command leaf is moved */

        else
        {
            struct ast_redir *ast_redir = &(*res)->data.ast_redir;

            if ((*node)->type == AST_COMMAND)
            {
                ast_redir->left = *node;
                *node = *res;
                *result = *res;
            }
            else
            {
                struct ast_redir *old_node = &(*node)->data.ast_redir;

                ast_redir->left = old_node->left;
                old_node->right = *res;
                old_node->left = NULL;
                *node = *res;
            }
        }
    }
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

    if (is_reserved(tok.value))
    {
        return P_KO;
    }

    /* Creating the command and placing it at the right place in the tree */

    tok = lexer_pop(lexer);

    struct ast *ast_cmd = ast_new(AST_COMMAND);

    ast_cmd_push(ast_cmd, tok.value, tok.exp);

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

    struct cmd_pack cmd_pack = { .node = &node, .result = &result };

    parse_elements_loop(res, lexer, &ast_cmd, cmd_pack);

    *res = result;

    return P_OK;
}
