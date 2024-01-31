#include "../ast/ast.h"
#include "domain_expansion.h"

struct traversal_functions
{
    enum ast_type type;
    void (*traversal_funs)(struct ast *ast);
};

static void expand_if(struct ast *ast)
{
    struct ast_if *ast_if = &ast->data.ast_if;
    expand(ast_if->condition);
    expand(ast_if->then_body);
    expand(ast_if->else_body);
}

static void expand_list(struct ast *ast)
{
    struct ast_list *ast_list = &ast->data.ast_list;
    for (int i = 0; ast_list->children[i] != NULL; i++)
    {
        expand(ast_list->children[i]);
    }
}

static void expand_pipe(struct ast *ast)
{
    struct ast_pipe *ast_pipe = &ast->data.ast_pipe;
    expand(ast_pipe->command);
    if (ast_pipe->child != NULL)
    {
        expand(ast_pipe->child);
    }
}

static void expand_while(struct ast *ast)
{
    struct ast_while *ast_while = &ast->data.ast_while;
    expand(ast_while->condition);
    expand(ast_while->body);
}

static void expand_until(struct ast *ast)
{
    struct ast_until *ast_until = &ast->data.ast_until;
    expand(ast_until->condition);
    expand(ast_until->body);
}

static void expand_ope(struct ast *ast)
{
    struct ast_ope *ast_ope = &ast->data.ast_ope;
    expand(ast_ope->left);
    expand(ast_ope->right);
}

static void expand_redir(struct ast *ast)
{
    struct ast_redir *ast_redir = &ast->data.ast_redir;
    if (!ast_redir->right)
    {
        expand(ast_redir->right);
    }
    expand(ast_redir->left);
}

static struct traversal_functions traversal_funs[] = {
    { AST_IF, expand_if },
    { AST_LIST, expand_list },
    { AST_PIPE, expand_pipe },
    { AST_WHILE, expand_while },
    { AST_UNTIL, expand_until },
    { AST_OPERATOR, expand_ope },
    { AST_REDIRECTION, expand_redir },
    { AST_COMMAND, expand_command }

};

void expand(struct ast *ast)
{
    if (ast == NULL)
    {
        return;
    }
    for (size_t i = 0;
         i < sizeof(traversal_funs) / sizeof(struct traversal_functions); i++)
    {
        if (ast->type == traversal_funs[i].type)
        {
            traversal_funs[i].traversal_funs(ast);
        }
    }
}
