#include "ast.h"

#include <err.h>
#include <stdlib.h>

struct ast *ast_new(enum ast_type type)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (!new)
    {
        return NULL;
    }

    new->type = type;
    new->args = calloc(1, sizeof(char *));
    new->children = calloc(1, sizeof(struct ast *));

    return new;
}

void ast_free(struct ast *ast)
{
    if (ast == NULL)
    {
        return;
    }

    if (ast->children != NULL)
    {
        for (int i = 0; ast->children[i] != NULL; i++)
        {
            ast_free(ast->children[i]);
        }
    }

    free(ast);
    free(ast->children);
    free(ast->args);
}

int ast_push_arg(struct ast *ast, char *arg)
{
    size_t len = 0;
    while (ast->args[len] != NULL)
    {
        len++;
    }
    len++;

    char **tmp = realloc(ast->args, sizeof(char *) * (len + 1));
    if (tmp == NULL)
    {
        return 1;
    }
    ast->args = tmp;

    ast->args[len - 1] = arg;
    ast->args[len] = NULL;

    return 0;
}

int ast_push_child(struct ast *ast, struct ast *child)
{
    size_t len = 0;
    while (ast->children[len] != NULL)
    {
        len++;
    }
    len++;

    struct ast **tmp = realloc(ast->children, sizeof(struct ast *) * (len + 1));
    if (tmp == NULL)
    {
        return 1;
    }
    ast->children = tmp;

    ast->children[len - 1] = child;
    ast->children[len] = NULL;

    return 0;
}
