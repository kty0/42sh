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

    if (ast_cmd->args[0] == NULL)
    {
        printf("NULL ");
    }

    for (int i = 0; ast_cmd->args[i] != NULL; i++)
    {
        printf("%s ", ast_cmd->args[i]);
    }

    printf("{ redirs: ");
    ast_print(ast_cmd->redirs);
    printf("} ");
    printf("{ var: ");
    ast_print(ast_cmd->vars);
    printf("} ");
}

static void print_list(struct ast *ast)
{
    struct ast_list *ast_list = &ast->data.ast_list;

    for (int i = 0; ast_list->children[i] != NULL; i++)
    {
        ast_print(ast_list->children[i]);
        printf(";");
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

static void print_assign(struct ast *ast)
{
    struct ast_assign *ast_assign = &ast->data.ast_assign;
    printf("{ ");
    printf("%s ", ast_assign->key);
    printf("= ");
    printf("%s ", ast_assign->value);
    printf("} ");

    ast_print(ast_assign->next);
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
    ast_print(ast_redir->next);
    printf("} ");
}

static void print_not(struct ast *ast)
{
    struct ast_not *ast_not = &ast->data.ast_not;

    printf("not ");
    ast_print(ast_not->child);
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
    case AST_ASSIGNMENT:
        print_assign(ast);
        break;
    case AST_NOT:
        print_not(ast);
        break;
    default:
        break;
    }
}
