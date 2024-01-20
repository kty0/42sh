#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

static void ast_free_cmd(struct ast *ast)
{
    struct ast_cmd *ast_cmd = &ast->data.ast_cmd;

    if (ast_cmd->args != NULL)
    {
        for (int i = 0; ast_cmd->args[i] != NULL; i++)
        {
            free(ast_cmd->args[i]);
        }
    }

    free(ast_cmd->args);
    free(ast);
}

static void ast_free_list(struct ast *ast)
{
    struct ast_list *ast_list = &ast->data.ast_list;

    if (ast_list->children != NULL)
    {
        for (int i = 0; ast_list->children[i] != NULL; i++)
        {
            ast_free(ast_list->children[i]);
        }
    }

    free(ast_list->children);
    free(ast);
}

static void ast_free_if(struct ast *ast)
{
    struct ast_if *ast_if = &ast->data.ast_if;

    ast_free(ast_if->condition);
    ast_free(ast_if->then_body);
    ast_free(ast_if->else_body);

    free(ast);
}

static void ast_free_not(struct ast *ast)
{
    struct ast_not *ast_not = &ast->data.ast_not;

    ast_free(ast_not->child);

    free(ast);
}

static void ast_free_pipe(struct ast *ast)
{
    struct ast_pipe *ast_pipe = &ast->data.ast_pipe;

    ast_free(ast_pipe->command);
    ast_free(ast_pipe->child);

    free(ast);
}

void ast_free(struct ast *ast)
{
    if (ast == NULL)
    {
        return;
    }

    switch (ast->type)
    {
    case AST_COMMAND:
        ast_free_cmd(ast);
        break;
    case AST_LIST:
        ast_free_list(ast);
        break;
    case AST_IF:
        ast_free_if(ast);
        break;
    case AST_NOT:
        ast_free_not(ast);
        break;
    case AST_PIPE:
        ast_free_pipe(ast);
        break;
    default:
        errx(1, "failed to free the AST");
    }
}