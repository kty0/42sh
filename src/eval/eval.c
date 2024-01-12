#include "eval.h"

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

static int eval_if(struct ast *ast);
static int eval_cmd(struct ast *ast);
static int eval_list(struct ast *ast);

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
    else
    {
        return eval_list(ast);
    }
}

static int eval_if(struct ast *ast)
{
    size_t i = 0;

    for (; ast->children[i] != NULL && ast->children[i + 1] != NULL; i += 2)
    {
        if (eval(ast->children[i]) == 0)
        {
            return eval(ast->children[i + 1]);
        }
    }

    if (ast->children[i] == NULL)
    {
        return 0;
    }

    return eval(ast->children[i]);
}

static int eval_cmd(struct ast *ast)
{
    if (strcmp(ast->args[0], "echo") == 0)
    {
        return echo(ast->args);
    }
    else if (strcmp(ast->args[0], "true") == 0)
    {
        return my_true();
    }
    else if (strcmp(ast->args[0], "false") == 0)
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
            return execvp(ast->args[0], ast->args);
        }

        int wstatus;
        waitpid(pid, &wstatus, 0);

        return WEXITSTATUS(wstatus);
    }
}

static int eval_list(struct ast *ast)
{
    int res = 0;

    for (int i = 0; ast->children[i] != NULL; i++)
    {
        res = eval(ast->children[i]);
    }

    return res;
}
