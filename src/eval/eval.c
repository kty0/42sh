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
#include "../built_in/built_in.h"
#include "../domain_expansion/domain_expansion.h"

struct eval_functions
{
    enum ast_type type;
    int (*eval_funs)(struct ast *ast);
};

struct function
{
    enum ast_type type;
    int (*fun)(struct ast *ast);
};

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

    for (size_t i = 0; ast_cmd->args[i]; i++)
    {
        expand(&(ast_cmd->args[i]));
    }
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
    else if (strcmp(ast_cmd->args[0], "cd") == 0)
    {
        int res = cd(ast_cmd->args);
        if (res == 1)
        {
            errx(1, "Usage : cd path/to/directory");
        }
        return res;
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
            if (execvp(ast_cmd->args[0], ast_cmd->args) == -1)
            {
                errx(127, "missing command");
            }
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

static int exec_fork(struct ast *ast, int fds[2], enum side s)
{
    if (ast == NULL)
    {
        exit(0);
    }

    pid_t pid = fork();
    if (pid != 0)
    {
        return pid;
    }

    int fdrep = s == LEFT ? STDOUT_FILENO : STDIN_FILENO;
    int fdpipe = s == LEFT ? fds[1] : fds[0];

    if (dup2(fdpipe, fdrep) == -1)
    {
        errx(1, "dup2 failed in while piping");
    }

    close(fds[0]);
    close(fds[1]);

    int res = eval(ast);

    exit(res);
}

static int eval_pipe(struct ast *ast)
{
    struct ast_pipe *ast_pipe = &ast->data.ast_pipe;

    if (ast_pipe->child == NULL)
    {
        return eval(ast_pipe->command);
    }

    int fds[2];
    if (pipe(fds) == -1)
   pick 150bed0 Revert "Merge branch '61-feat-parser-ad
    {
        errx(1, "failed to create the pipe");
    }

    int pid_left = exec_fork(ast_pipe->command, fds, LEFT);
    int pid_right = exec_fork(ast_pipe->child, fds, RIGHT);

    close(fds[0]);
    close(fds[1]);

    int wstatus;
    waitpid(pid_left, &wstatus, 0);
    waitpid(pid_right, &wstatus, 0);

    return WEXITSTATUS(wstatus);
}

static int eval_while(struct ast *ast)
{
    struct ast_while *ast_while = &ast->data.ast_while;

    int res = 0;

    while (!eval(ast_while->condition))
    {
        res = eval(ast_while->body);
    }

    return res;
}

static int eval_until(struct ast *ast)
{
    struct ast_until *ast_until = &ast->data.ast_until;

    int res = 0;

    while (eval(ast_until->condition))
    {
        res = eval(ast_until->body);
    }

    return res;
}

static int eval_ope(struct ast *ast)
{
    struct ast_ope *ast_ope = &ast->data.ast_ope;

    int res = eval(ast_ope->left);

    if (ast_ope->right != NULL)
    {
        if (ast_ope->type == AND)
        {
            if (res)
            {
                return res;
            }

            return eval(ast_ope->right);
        }
        else
        {
            if (!res)
            {
                return res;
            }

            return eval(ast_ope->right);
        }
    }

    return res;
}

static struct eval_functions eval_funs[] = {
    { AST_IF, eval_if },        { AST_NOT, eval_not },
    { AST_LIST, eval_list },    { AST_PIPE, eval_pipe },
    { AST_WHILE, eval_while },  { AST_UNTIL, eval_until },
    { AST_OPERATOR, eval_ope }, { AST_REDIRECTION, eval_redir },
    { AST_COMMAND, eval_cmd }
};

int eval(struct ast *ast)
{
    if (ast == NULL)
    {
        return 0;
    }
    for (size_t i = 0; sizeof(eval_funs) / sizeof(struct eval_functions); i++)
    {
        if (ast->type == eval_funs[i].type)
        {
            return eval_funs[i].eval_funs(ast);
        }
    }
    err(1, "invalid node in AST");
}
