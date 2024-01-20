#include "ast_new.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

struct ast *ast_new(enum ast_type type)
{
    switch (type)
    {
    case AST_COMMAND:
        return ast_new_cmd();
    case AST_LIST:
        return ast_new_list();
    case AST_IF:
        return ast_new_if();
    case AST_NOT:
        return ast_new_not();
    case AST_PIPE:
        return ast_new_pipe();
    default:
        return NULL;
    }
}

struct ast *ast_new_cmd(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new ast");
    }

    new->type = AST_COMMAND;

    struct ast_cmd *ast_cmd = &new->data.ast_cmd;

    ast_cmd->args = calloc(1, sizeof(char *));
    if (ast_cmd->args == NULL)
    {
        errx(1, "failed to create a new ast");
    }

    return new;
}

struct ast *ast_new_if(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (!new)
    {
        errx(1, "failed to create a new ast");
    }

    new->type = AST_IF;

    return new;
}

struct ast *ast_new_list(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new ast");
    }

    new->type = AST_LIST;

    struct ast_list *ast_list = &new->data.ast_list;

    ast_list->children = calloc(1, sizeof(struct ast *));
    if (ast_list->children == NULL)
    {
        errx(1, "failed to create a new ast");
    }

    return new;
}

struct ast *ast_new_not(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new ast");
    }

    new->type = AST_NOT;

    return new;
}

struct ast *ast_new_pipe(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new ast");
    }

    new->type = AST_PIPE;

    return new;
}
