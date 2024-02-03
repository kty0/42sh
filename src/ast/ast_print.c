#include "ast_print.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lexer/token.h"
#include "ast.h"

struct print_functions
{
    enum ast_type type;
    void (*print_funs)(struct ast *ast);
};

void print_if(struct ast *ast)
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

void print_command(struct ast *ast)
{
    struct ast_cmd *ast_cmd = &ast->data.ast_cmd;

    printf("command ");

    for (int i = 0; ast_cmd->args[i] != NULL; i++)
    {
        printf("%s ", ast_cmd->args[i]);
    }
}

void print_list(struct ast *ast)
{
    struct ast_list *ast_list = &ast->data.ast_list;

    for (int i = 0; ast_list->children[i] != NULL; i++)
    {
        ast_print(ast_list->children[i]);
    }
}

void print_pipe(struct ast *ast)
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

void print_while(struct ast *ast)
{
    struct ast_while *ast_while = &ast->data.ast_while;

    printf("while { ");
    ast_print(ast_while->condition);
    printf("do { ");
    ast_print(ast_while->body);
    printf("} ");
    printf("done } ");
}

void print_until(struct ast *ast)
{
    struct ast_until *ast_until = &ast->data.ast_until;

    printf("until { ");
    ast_print(ast_until->condition);
    printf("do { ");
    ast_print(ast_until->body);
    printf("} ");
    printf("done } ");
}

void print_ope(struct ast *ast)
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

void print_redir(struct ast *ast)
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
    if (ast_redir->next != NULL)
    {
        ast_print(ast_redir->next);
    }
    else
    {
        ast_print(ast_redir->next);
    }
    printf("} ");
}

void print_for(struct ast *ast)
{
    struct ast_for *ast_for = &ast->data.ast_for;
    printf("for ");
    printf("%s (", ast_for->var);
    for (size_t i = 0; ast_for->list[i]; i++)
    {
        printf(" %s ", ast_for->list[i]);
    }
    printf(") do ");
    ast_print(ast_for->body);
}

static struct print_functions print_funs[] = {
    { AST_IF, print_if },        { AST_COMMAND, print_command },
    { AST_LIST, print_list },    { AST_PIPE, print_pipe },
    { AST_WHILE, print_while },  { AST_UNTIL, print_until },
    { AST_OPERATOR, print_ope }, { AST_REDIRECTION, print_redir },
    { AST_FOR, print_for }
};

void ast_print(struct ast *ast)
{
    if (ast == NULL)
    {
        return;
    }
    for (size_t i = 0; i < sizeof(print_funs) / sizeof(struct print_functions);
         i++)
    {
        if (ast->type == print_funs[i].type)
        {
            print_funs[i].print_funs(ast);
        }
    }
}
