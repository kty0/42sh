#include "ast.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
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

void print(struct ast *ast)
{
    switch (ast->type)
    {
    case AST_IF:
        print_if(ast);
        break;
    case AST_COMMAND:
        print_command(ast);
        break;
    case AST_LIST:
        print_list(ast);
        break;
    default:
        break;
    }
}

void print_list(struct ast *ast)
{
    assert(ast->type == AST_LIST);
    for (int i = 0; ast->children[i] != NULL; i++)
    {
        print(ast->children[i]);
    }
}

void print_if(struct ast *ast)
{
    assert(ast->type == AST_IF);
    printf("if { ");
    print(ast->children[0]);
    printf("}; then {");
    print(ast->children[1]);
    printf("}");
    int i = 2;
    for (; ast->children[i] != NULL && ast->children[i + 1] != NULL; i += 2)
    {
        printf("elif {");
        print(ast->children[i]);
        printf("}; then {");
        print(ast->children[i + 1]);
        printf("}");
    }
    if (ast->children[i] != NULL)
    {
        printf("else {");
        print(ast->children[i]);
        printf("}");
    }
    printf("fi \n");
}

void print_command(struct ast *ast)
{
    assert(ast->type == AST_COMMAND);
    printf(" command ");
    int i = 0;
    while (ast->args[i] != NULL)
    {
        printf("%s ", ast->args[i]);
        i++;
    }
}
