#include "ast_new.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

struct ast *ast_new_cmd(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new cmd ast");
    }

    new->type = AST_COMMAND;

    struct ast_cmd *ast_cmd = &new->data.ast_cmd;

    ast_cmd->args = calloc(1, sizeof(char *));
    if (ast_cmd->args == NULL)
    {
        errx(1, "failed to create a new cmd ast");
    }

    return new;
}

struct ast *ast_new_if(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (!new)
    {
        errx(1, "failed to create a new if ast");
    }

    new->type = AST_IF;

    return new;
}

struct ast *ast_new_list(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new list ast");
    }

    new->type = AST_LIST;

    struct ast_list *ast_list = &new->data.ast_list;

    ast_list->children = calloc(1, sizeof(struct ast *));
    if (ast_list->children == NULL)
    {
        errx(1, "failed to create a new list ast");
    }

    return new;
}

struct ast *ast_new_not(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new not ast");
    }

    new->type = AST_NOT;

    return new;
}

struct ast *ast_new_pipe(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new pipe ast");
    }

    new->type = AST_PIPE;

    return new;
}

struct ast *ast_new_while(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new while ast");
    }

    new->type = AST_WHILE;

    return new;
}

struct ast *ast_new_until(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new until ast");
    }

    new->type = AST_UNTIL;

    return new;
}

struct ast *ast_new_ope(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new ope ast");
    }

    new->type = AST_OPERATOR;

    return new;
}

struct ast *ast_new_redir(void)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (new == NULL)
    {
        errx(1, "failed to create a new redir ast");
    }

    new->type = AST_REDIRECTION;

    return new;

}

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
    case AST_WHILE:
        return ast_new_while();
    case AST_UNTIL:
        return ast_new_until();
    case AST_OPERATOR:
        return ast_new_ope();
    case AST_REDIRECTION:
        return ast_new_redir();
    case AST_WORD:
        return ast_new_word();
    default:
        return NULL;
    }
}
