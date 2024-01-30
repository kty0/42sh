#include "ast.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lexer/token.h"

int ast_cmd_push(struct ast *ast, char *arg, enum exp_type exp)
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

    len = 0;
    while (ast_cmd->exps[len] != NULL)
    {
        len++;
    }
    len++;

    enum exp_type **tmp2 =
        realloc(ast_cmd->exps, sizeof(enum exp_type *) * (len + 1));
    if (tmp2 == NULL)
    {
        return 1;
    }
    ast_cmd->exps = tmp2;

    enum exp_type *new_exp = malloc(sizeof(enum exp_type));
    *new_exp = exp;

    ast_cmd->exps[len - 1] = new_exp;
    ast_cmd->exps[len] = NULL;

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

static void print_pipe(struct ast *ast)
{
    struct ast_pipe *ast_pipe = &ast->data.ast_pipe;

    printf("{ ");
    ast_print(ast_pipe->command);
    if (ast_pipe->child != NULL)
    {
        printf("| ");
        ast_print(ast_pipe->child);
    }
    printf("} ");
}

static void print_while(struct ast *ast)
{
    struct ast_while *ast_while = &ast->data.ast_while;

    printf("while { ");
    ast_print(ast_while->condition);
    printf("do { ");
    ast_print(ast_while->body);
    printf("} ");
    printf("done } ");
}

static void print_until(struct ast *ast)
{
    struct ast_until *ast_until = &ast->data.ast_until;

    printf("until { ");
    ast_print(ast_until->condition);
    printf("do { ");
    ast_print(ast_until->body);
    printf("} ");
    printf("done } ");
}

static void print_ope(struct ast *ast)
{
    struct ast_ope *ast_ope = &ast->data.ast_ope;

    printf("{ ");
    ast_print(ast_ope->left);
    if (ast_ope->right != NULL)
    {
        if (ast_ope->type == AND)
        {
            printf("AND ");
        }
        else
        {
            printf("OR ");
        }
    }
    ast_print(ast_ope->right);
    printf("} ");
}

static void print_redir(struct ast *ast)
{
    struct ast_redir *ast_redir = &ast->data.ast_redir;
    printf("{ ");
    printf("%d ", ast_redir->fd);
    switch (ast_redir->type)
    {
    case LESS:
        printf("< ");
        break;
    case GREAT:
        printf("> ");
        break;
    case LESSAND:
        printf("<& ");
        break;
    case GREATAND:
        printf(">& ");
        break;
    case LESSGREAT:
        printf("<> ");
        break;
    case DGREAT:
        printf(">> ");
        break;
    case CLOBBER:
        printf(">| ");
        break;
    default:
        break;
    }
    if (ast_redir->right != NULL)
    {
        ast_print(ast_redir->right);
    }
    else
    {
        ast_print(ast_redir->left);
    }
    printf("} ");
}

void ast_print(struct ast *ast)
{
    if (ast == NULL)
    {
        return;
    }

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
    case AST_PIPE:
        print_pipe(ast);
        break;
    case AST_WHILE:
        print_while(ast);
        break;
    case AST_UNTIL:
        print_until(ast);
        break;
    case AST_OPERATOR:
        print_ope(ast);
        break;
    case AST_REDIRECTION:
        print_redir(ast);
        break;
    default:
        break;
    }
}
