#include "ast.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lexer/token.h"

/**
 *  \brief  Adds an assignment node to the linked list of a given command
 *  \fn     int ast_cmd_push_assign(struct ast *ast, struct ast *assignment);
 *  \param  ast The destination command
 *  \param  assignment The assignment node to be pushed
 *  \return 0 or 1 whether it succeeded or not
 */
int ast_cmd_push_assign(struct ast *ast, struct ast *assignment)
{
    struct ast_cmd *ast_cmd = &ast->data.ast_cmd;

    if (ast_cmd->vars == NULL)
    {
        ast_cmd->vars = assignment;
    }
    else
    {
        struct ast_assign *node = &(ast_cmd->vars)->data.ast_assign;

        while (node->next != NULL)
        {
            node = &(node->next)->data.ast_assign;
        }

        node->next = assignment;
    }

    return 0;
}

/**
 *  \brief  Adds a redir node to the linked list of a given command
 *  \fn     int ast_cmd_push_redir(struct ast *ast, struct ast *redir);
 *  \param  ast The destination command
 *  \param  redir The redir node to be pushed
 *  \return 0 or 1 whether it succeeded or not
 */
int ast_cmd_push_redir(struct ast *ast, struct ast *redir)
{
    struct ast_cmd *ast_cmd = &ast->data.ast_cmd;

    if (ast_cmd->redirs == NULL)
    {
        ast_cmd->redirs = redir;
    }
    else
    {
        struct ast_redir *node = &(ast_cmd->redirs)->data.ast_redir;

        while (node->next != NULL)
        {
            node = &(node->next)->data.ast_redir;
        }

        node->next = redir;
    }

    return 0;
}

/**
 *  \brief  Adds an argument to the NULL terminated args of a given command
 *  \fn     int ast_cmd_push(struct ast *ast, char *arg);
 *  \param  ast The destination command
 *  \param  arg The argument to be pushed
 *  \return 0 or 1 whether it succeeded or not
 */
int ast_cmd_push(struct ast *ast, char *arg)
{
    struct ast_cmd *ast_cmd = &ast->data.ast_cmd;

    size_t len = 0;
    while (ast_cmd->args[len] != NULL)
    {
        len++;
    }
    len++;

    char **tmp = realloc(ast_cmd->args, sizeof(char *) * (len + 1));
    if (tmp == NULL)
    {
        return 1;
    }
    ast_cmd->args = tmp;

    ast_cmd->args[len - 1] = arg;
    ast_cmd->args[len] = NULL;

    return 0;
}

/**
 *  \brief  Adds a child note to the NULL terminated children of a given command
 *  \fn     int ast_list_push(struct ast *ast, struct ast *child);
 *  \param  ast The destination command
 *  \param  child The child to be pushed
 *  \return 0 or 1 whether it succeeded or not
 */
int ast_list_push(struct ast *ast, struct ast *child)
{
    struct ast_list *ast_list = &ast->data.ast_list;

    size_t len = 0;
    while (ast_list->children[len] != NULL)
    {
        len++;
    }
    len++;

    struct ast **tmp =
        realloc(ast_list->children, sizeof(struct ast *) * (len + 1));
    if (tmp == NULL)
    {
        return 1;
    }
    ast_list->children = tmp;

    ast_list->children[len - 1] = child;
    ast_list->children[len] = NULL;

    return 0;
}

int ast_for_push(struct ast *ast, char *item)
{
    struct ast_for *ast_for = &ast->data.ast_for;

    size_t len = 0;
    while (ast_for->list[len] != NULL)
    {
        len++;
    }
    len++;

    char **tmp = realloc(ast_for->list, sizeof(char *) * (len + 1));
    if (tmp == NULL)
    {
        return 1;
    }
    ast_for->list = tmp;

    ast_for->list[len - 1] = item;
    ast_for->list[len] = NULL;

    return 0;
}
