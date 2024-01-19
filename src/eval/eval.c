#include "eval.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../built_in/echo.h"
#include "../built_in/true_false.h"

static int eval_if(struct ast *ast)
{
    struct ast_if *ast_if = &ast->data.ast_if;

    if (eval(ast_if->condition) == 0)
    {
        return eval(ast_if->then_body);
    }

    return eval(ast_if->else_body);
}

static int eval_cmd(struct ast *ast)
{
    struct ast_cmd *ast_cmd = &ast->data.ast_cmd;

    if (strcmp(ast_cmd->args[0], "echo") == 0)
    {
        int res = echo(ast_cmd->args);
        fflush(stdout);
        return res;
    }
    else if (strcmp(ast_cmd->args[0], "true") == 0)
    {
        return my_true();
    }
    else if (strcmp(ast_cmd->args[0], "false") == 0)
    {
        return my_false();
    }
    else
    {
        int pid = fork();
        if (pid == -1)
        {
            return 1;
        }

        if (pid == 0)
        {
            errx(execvp(ast_cmd->args[0], ast_cmd->args), "missing command");
        }

        int wstatus;
        waitpid(pid, &wstatus, 0);

        return WEXITSTATUS(wstatus);
    }
}

static int eval_list(struct ast *ast)
{
    struct ast_list *ast_list = &ast->data.ast_list;

    int res = 0;

    for (int i = 0; ast_list->children[i] != NULL; i++)
    {
        res = eval(ast_list->children[i]);
    }

    return res;
}

static int eval_not(struct ast *ast)
{
    struct ast_not *ast_not = &ast->data.ast_not;

    return !eval(ast_not->child);
}

int eval(struct ast *ast)
{
    if (ast == NULL)
    {
        return 0;
    }

    if (ast->type == AST_IF)
    {
        return eval_if(ast);
    }
    else if (ast->type == AST_COMMAND)
    {
        return eval_cmd(ast);
    }
    else if (ast->type == AST_NOT)
    {
        return eval_not(ast);
    }
    else
    {
        return eval_list(ast);
    }

    err(1, "invalid node in the AST");
}
