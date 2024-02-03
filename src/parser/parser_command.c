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
        || !strcmp(value, "elif") || !strcmp(value, "!") || !strcmp(value, "}");
}

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

enum parser_status parse_command(struct ast **res, struct lexer *lexer)
{
    return parse_shell_command(res, lexer) == P_OK
            || (parse_simple_command(res, lexer) == P_OK && *res != NULL)
        ? P_OK
        : P_KO;
}

enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "{"))
    {
        return parse_rule_if(res, lexer) == P_OK
                || parse_rule_while(res, lexer) == P_OK
                || parse_rule_until(res, lexer) == P_OK
            ? P_OK
            : P_KO;
    }

    tok = lexer_pop_free(lexer);

    if (parse_compound_list(res, lexer) != P_OK)
    {
        return P_KO;
    }

    tok = lexer_peek(lexer);

    if (tok.type != TOKEN_WORD || strcmp(tok.value, "}"))
    {
        return P_KO;
    }

    tok = lexer_pop_free(lexer);

    return P_OK;
}

/* Unintuitively parsing a not so simple command:
 *
 * architecture of the Command shall be a single command node with
 * - its Arguments in an array of char *
 * - its Redirections in an array of ast_redir as ast *
 * - its Variables in an array of ast_assign as ast *
 * all arrays being NULL terminated
 *
 *              ╭─────── C ───────╮
 *              │        │        │
 *              A        R        V
 *              │        │        │
 *              A        R        V
 *                       │        │
 *                       R        V
 *                       │
 *                       R
 *                       │
 *                       R
 *
 */
enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer)
{
    *res = NULL;

    /* Creating the command root node */

    struct ast *root = ast_new(AST_COMMAND);

    int is_empty = 1;

    /* Parsing early redirections or assignments */

    while (parse_prefix(res, lexer) != P_KO)
    {
        is_empty = 0;

        if ((*res)->type == AST_REDIRECTION)
        {
            ast_cmd_push_redir(root, *res);
        }
        else
        {
            ast_cmd_push_assign(root, *res);
        }
    }

    struct token tok = lexer_peek(lexer);

    /* If there is no word coming then we only have prefixes and may be good */

    if (tok.type != TOKEN_WORD)
    {
        if (is_empty)
        {
            ast_free(root);
        }
        else
        {
            *res = root;
        }

        return P_OK;
    }

    /* If this word is reserved though, it cannot be a command */

    if (is_reserved(tok.value) || is_assignment(tok.value))
    {
        ast_free(root);
        return P_KO;
    }

    /* Building up the command */

    tok = lexer_pop(lexer);

    ast_cmd_push(root, tok.value);

    /* Parsing the following elements */

    while (parse_element(res, lexer) != P_KO)
    {
        /* If it's a word it goes in the command */

        if ((*res)->type == AST_WORD)
        {
            struct ast_word *word = &(*res)->data.ast_word;
            ast_cmd_push(root, word->arg);
            free(*res);
        }

        /* Else it is appended to the redirections */

        else
        {
            ast_cmd_push_redir(root, *res);
        }
    }

    *res = root;

    return P_OK;
}
