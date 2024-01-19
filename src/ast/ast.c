#include "ast.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

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

static void print_if(struct ast *ast)
{
    struct ast_if *ast_if = &ast->data.ast_if;

    printf("if { ");
    ast_print(ast_if->condition);
    printf("} ");

    printf("then {");
    ast_print(ast_if->then_body);
    printf("} ");

    if (ast_if->else_body != NULL)
    {
        printf("else {");
        ast_print(ast_if->else_body);
        printf("} ");
    }

    printf("fi ");
}

static void print_command(struct ast *ast)
{
    struct ast_cmd *ast_cmd = &ast->data.ast_cmd;

    printf("command ");

    for (int i = 0; ast_cmd->args[i] != NULL; i++)
    {
        printf("%s ", ast_cmd->args[i]);
    }
}

static void print_list(struct ast *ast)
{
    struct ast_list *ast_list = &ast->data.ast_list;

    for (int i = 0; ast_list->children[i] != NULL; i++)
    {
        ast_print(ast_list->children[i]);
    }
}

void ast_print(struct ast *ast)
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
