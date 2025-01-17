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

    ast_free(ast_cmd->redirs);
    ast_free(ast_cmd->vars);

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

static void ast_free_while(struct ast *ast)
{
    struct ast_while *ast_while = &ast->data.ast_while;

    ast_free(ast_while->condition);
    ast_free(ast_while->body);

    free(ast);
}

static void ast_free_until(struct ast *ast)
{
    struct ast_until *ast_until = &ast->data.ast_until;

    ast_free(ast_until->condition);
    ast_free(ast_until->body);

    free(ast);
}

static void ast_free_ope(struct ast *ast)
{
    struct ast_ope *ast_ope = &ast->data.ast_ope;

    ast_free(ast_ope->left);
    ast_free(ast_ope->right);

    free(ast);
}

static void ast_free_redir(struct ast *ast)
{
    struct ast_redir *ast_redir = &ast->data.ast_redir;

    ast_free(ast_redir->next);

    free(ast_redir->file);
    free(ast);
}

static void ast_free_assign(struct ast *ast)
{
    struct ast_assign *ast_assign = &ast->data.ast_assign;

    ast_free(ast_assign->next);

    free(ast_assign->key);
    free(ast_assign->value);
}

static void ast_free_for(struct ast *ast)
{
    struct ast_for *ast_for = &ast->data.ast_for;

    free(ast_for->var);

    if (ast_for->list != NULL)
    {
        for (int i = 0; ast_for->list[i] != NULL; i++)
        {
            free(ast_for->list[i]);
        }
    }
    free(ast_for->list);

    ast_free(ast_for->body);

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
    case AST_WHILE:
        ast_free_while(ast);
        break;
    case AST_UNTIL:
        ast_free_until(ast);
        break;
    case AST_OPERATOR:
        ast_free_ope(ast);
        break;
    case AST_REDIRECTION:
        ast_free_redir(ast);
        break;
    case AST_ASSIGNMENT:
        ast_free_assign(ast);
        break;
    case AST_FOR:
        ast_free_for(ast);
        break;
    default:
        errx(2, "failed to free the AST");
    }
}
