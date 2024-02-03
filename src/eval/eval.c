#define _POSIX_C_SOURCE 200809L

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
#include "../hash_map/hash_map.h"

static int is_env = 0;
static struct hash_map *hash_map = NULL;

struct eval_functions
{
    enum ast_type type;
    int (*eval_funs)(struct ast *ast);
};

static int eval_if(struct ast *ast)
{
    struct ast_if *ast_if = &ast->data.ast_if;

    if (eval(ast_if->condition, NULL) == 0)
    {
        return eval(ast_if->then_body, NULL);
    }

    return eval(ast_if->else_body, NULL);
}

static void free_mat(char **mat)
{
    for (int i = 0; mat[i] != NULL; i++)
    {
        free(mat[i]);
    }
    free(mat);
}

int launch_command(struct ast_cmd *ast_cmd)
{
    /* Creating a copy of the arguments but expanded */
    char **new_args = expand(ast_cmd->args, hash_map);
    if (new_args == NULL)
    {
        return 2;
    }

    if (new_args[0] == NULL)
    {
        free_mat(new_args);
        return 0;
    }

    if (strcmp(new_args[0], "echo") == 0)
    {
        int res = echo(new_args);
        fflush(stdout);
        free_mat(new_args);
        return res;
    }
    else if (strcmp(new_args[0], "true") == 0)
    {
        free_mat(new_args);
        return my_true();
    }
    else if (strcmp(new_args[0], "false") == 0)
    {
        free_mat(new_args);
        return my_false();
    }
    else if (strcmp(new_args[0], "cd") == 0)
    {
        int res = cd(new_args);
        if (res == 1)
        {
            errx(1, "Usage : cd path/to/directory");
        }
        free_mat(new_args);
        return res;
    }
    else if (strcmp(ast_cmd->args[0], "exit") == 0)
    {
        int res = my_exit(ast_cmd->args);
        if (res == -1)
        {
            errx(1, "Usage : exit <0-255>");
        }
        return res;
    }
    else
    {
        int pid = fork();
        if (pid == -1)
        {
            free_mat(new_args);
            return 1;
        }

        if (pid == 0)
        {
            if (execvp(new_args[0], new_args) == -1)
            {
                free_mat(new_args);
                errx(127, "missing command");
            }
        }

        int wstatus;
        waitpid(pid, &wstatus, 0);

        free_mat(new_args);
        return WEXITSTATUS(wstatus);
    }
}

static int eval_cmd(struct ast *ast)
{
    struct ast_cmd *ast_cmd = &ast->data.ast_cmd;

    is_env = ast_cmd->args[0] != NULL;

    eval(ast_cmd->vars, NULL);

    return eval_redir(ast_cmd->redirs, ast_cmd);
}

static int eval_list(struct ast *ast)
{
    struct ast_list *ast_list = &ast->data.ast_list;

    int res = 0;

    for (int i = 0; ast_list->children[i] != NULL; i++)
    {
        res = eval(ast_list->children[i], NULL);
    }

    return res;
}

static int eval_not(struct ast *ast)
{
    struct ast_not *ast_not = &ast->data.ast_not;

    return !eval(ast_not->child, NULL);
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

    int res = eval(ast, NULL);

    exit(res);
}

static int eval_pipe(struct ast *ast)
{
    struct ast_pipe *ast_pipe = &ast->data.ast_pipe;

    if (ast_pipe->child == NULL)
    {
        return eval(ast_pipe->command, NULL);
    }

    int fds[2];
    if (pipe(fds) == -1)
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

    while (!eval(ast_while->condition, NULL))
    {
        res = eval(ast_while->body, NULL);
    }

    return res;
}

static int eval_until(struct ast *ast)
{
    struct ast_until *ast_until = &ast->data.ast_until;

    int res = 0;

    while (eval(ast_until->condition, NULL))
    {
        res = eval(ast_until->body, NULL);
    }

    return res;
}

static int eval_for(struct ast *ast)
{
    struct ast_for *ast_for = &ast->data.ast_for;

    int res = 0;
    char **array = NULL;
    int updated = 0;

    for (size_t i = 0; ast_for->list[i]; i++)
    {
        array = calloc(1, sizeof(char *));
        array = expand_string(ast_for->list[i], array);
        if (array == NULL)
        {
            return 1;
        }
        free(ast_for->list[i]);
        ast_for->list[i] = array[0];
        free(array);
        hash_map_insert(hash_map, ast_for->var, ast_for->list[i], &updated);
        res = eval(ast_for->body, NULL);
    }
    return res;
}

static int eval_ope(struct ast *ast)
{
    struct ast_ope *ast_ope = &ast->data.ast_ope;

    int res = eval(ast_ope->left, NULL);

    if (ast_ope->right != NULL)
    {
        if (ast_ope->type == AND)
        {
            if (res)
            {
                return res;
            }

            return eval(ast_ope->right, NULL);
        }
        else
        {
            if (!res)
            {
                return res;
            }

            return eval(ast_ope->right, NULL);
        }
    }

    return res;
}

static int eval_assign(struct ast *ast)
{
    struct ast_assign *ast_assign = &ast->data.ast_assign;

    /* Weird trick to expand a single string easily */
    char **array = calloc(1, sizeof(char *));
    array = expand_string(ast_assign->value, array);
    if (array == NULL)
    {
        return 1;
    }
    free(ast_assign->value);
    ast_assign->value = array[0];
    free(array);

    if (is_env)
    {
        return setenv(ast_assign->key, ast_assign->value, 1) == -1 ? 1 : 0;
    }

    int updated = 0;

    hash_map_insert(hash_map, ast_assign->key, ast_assign->value, &updated);

    return eval(ast_assign->next, NULL);
}

static struct eval_functions eval_funs[] = {
    { AST_IF, eval_if },        { AST_NOT, eval_not },
    { AST_LIST, eval_list },    { AST_PIPE, eval_pipe },
    { AST_WHILE, eval_while },  { AST_UNTIL, eval_until },
    { AST_OPERATOR, eval_ope }, { AST_ASSIGNMENT, eval_assign },

    { AST_COMMAND, eval_cmd },  { AST_FOR, eval_for },
};

int eval(struct ast *ast, struct hash_map *h)
{
    if (ast == NULL)
    {
        return 0;
    }

    if (hash_map == NULL)
    {
        hash_map = h;
    }

    for (size_t i = 0; i < sizeof(eval_funs) / sizeof(struct eval_functions);
         i++)
    {
        if (ast->type == eval_funs[i].type)
        {
            return eval_funs[i].eval_funs(ast);
        }
    }

    err(1, "invalid node in AST");
}
