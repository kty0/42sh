#include <err.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../built_in/built_in.h"
#include "eval.h"

static struct ast_cmd *command = NULL;

static int eval_redir_basic(struct ast_redir *ast_redir, int flags)
{
    int fd = open(ast_redir->file, flags, 0644);
    if (fd == -1)
    {
        return 1;
    }

    /* Saving the fd which will be overwritten */

    int fdout = dup(ast_redir->fd);

    /* Overwriting the fd with the new one */

    int fdtemp = dup2(fd, ast_redir->fd);
    if (fdtemp == -1)
    {
        close(fd);
        close(fdout);

        return 1;
    }

    close(fd);

    /* Pursuing the evaluation */

    int res = eval_redir(ast_redir->next, NULL);

    /* Putting back the overwritten fd as it was */

    if (fdout != -1)
    {
        if (dup2(fdout, ast_redir->fd) == -1)
        {
            close(fdtemp);
            return 1;
        }

        close(fdout);
    }

    return res;
}

/* Not working still in progress code */
static int eval_redir_anyand(struct ast_redir *ast_redir)
{
    /* If file is "-" just closes the given fd */

    if (!strcmp(ast_redir->file, "-"))
    {
        close(ast_redir->fd);
        return 0;
    }

    /* Saving the fd which will be overwritten */

    int fdout = dup(ast_redir->fd);
    if (fdout == -1)
    {
        return 1;
    }

    int fdsrc = 0;
    if (strcmp(ast_redir->file, "0"))
    {
        fdsrc = atoi(ast_redir->file);
        if (fdsrc == 0)
        {
            errx(1, "source file descriptor is weird");
        }
    }

    /* Overwriting the fd with the new one */

    int fdtemp = dup2(fdsrc, ast_redir->fd);
    if (fdtemp == -1)
    {
        close(fdsrc);
        close(fdout);

        errx(1, "source file descriptor is invalid");
    }

    /* Pursuing the evaluation */

    int res = eval_redir(ast_redir->next, NULL);

    /* Putting back the overwritten fd as it was */

    if (dup2(fdout, ast_redir->fd) == -1)
    {
        close(fdtemp);
        return 1;
    }

    close(fdout);

    return res;
}

int eval_redir(struct ast *ast, struct ast_cmd *ast_cmd)
{
    if (ast_cmd != NULL)
    {
        command = ast_cmd;
    }

    if (ast == NULL)
    {
        return launch_command(command);
    }

    struct ast_redir *ast_redir = &ast->data.ast_redir;

    if (ast_redir->type == LESS)
    {
        return eval_redir_basic(ast_redir, O_RDONLY);
    }
    else if (ast_redir->type == GREAT || ast_redir->type == CLOBBER)
    {
        return eval_redir_basic(ast_redir, O_WRONLY | O_CREAT | O_TRUNC);
    }
    else if (ast_redir->type == LESSAND || ast_redir->type == GREATAND)
    {
        return eval_redir_anyand(ast_redir);
    }
    else if (ast_redir->type == LESSGREAT)
    {
        return eval_redir_basic(ast_redir, O_RDWR | O_CREAT | O_TRUNC);
    }
    else if (ast_redir->type == DGREAT)
    {
        return eval_redir_basic(ast_redir, O_WRONLY | O_CREAT | O_APPEND);
    }
    else
    {
        errx(1, "invalid redir node");
    }
}
