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

static int eval_if(struct ast *ast);
static int eval_cmd(struct ast *ast);
static int eval_list(struct ast *ast);
static int eval_not(struct ast *ast);
static int eval_pipe(struct ast *ast);

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
    else if (ast->type == AST_PIPE)
    {
        return eval_pipe(ast);
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
        int res = echo(ast->args);
        fflush(stdout);
        return res;
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
            errx(execvp(ast->args[0], ast->args), "missing command");
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

static int eval_not(struct ast *ast)
{
    return !eval(ast->children[0]);
}

static int exec_fork(char **argv, int *fds, enum side s)
{
    pid_t pid = fork();
    if (pid != 0)
    {
        return pid;
    }

    if (s == LEFT)
    {
        dup2(fds[0], STDOUT_FILENO);

        close(fds[0]);
    }
    else if (s == MID)
    {
        dup2(fds[0], STDIN_FILENO);
        dup2(fds[1], STDOUT_FILENO);

        close(fds[0]);
        close(fds[1]);
    }
    else
    {
        dup2(fds[0], STDIN_FILENO);

        close(fds[0]);
    }

    execvp(argv[0], argv);

    errx(1, "execvp failed");
}

static int eval_pipe(struct ast *ast)
{
    int n = 10;

    int *fds = malloc(2 * (n - 1) * sizeof(int));

    int pid = -1;

    for (int i = 0; i < n; i++)
    {
        if (pipe(fds + i * 2) == -1)
        {
            errx(1, "issue while piping");
        }

        enum side pos = i == n - 1 ? RIGHT : MID;

        if (i == 0)
        {
            pid = exec_fork(ast->children[i]->args, 0, LEFT);
        }
        else
        {
            pid = exec_fork(ast->children[i]->args + i, fds + (i - 1) * 2 + 1,
                            pos);
        }
    }

    for (int i = 0; i < 2 * (n - 1); i++)
    {
        close(fds[i]);
    }

    free(fds);

    int wstatus;
    waitpid(pid, &wstatus, 0);

    return WEXITSTATUS(wstatus);
}
