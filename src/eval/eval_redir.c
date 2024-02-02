#include <err.h>
#include <fcntl.h>
#include <limits.h>
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

/* Opens file for reading on the designated file descriptor */
static int eval_redir_less(struct ast_redir *ast_redir)
{
    int fd = open(ast_redir->file, O_RDONLY);
    if (fd == -1)
    {
        return 1;
    }

    /* Saving the fd which will be overwritten */

    int fdout = dup(ast_redir->fd);
    if (fdout == -1)
    {
        close(fd);

        return 1;
    }

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

    int res;
    if (ast_redir->right == NULL)
    {
        res = eval(ast_redir->left);
    }
    else
    {
        res = eval(ast_redir->right);
    }

    /* Putting back the overwritten fd as it was */

    if (dup2(fdout, ast_redir->fd) == -1)
    {
        close(fdtemp);
        return 1;
    }

    close(fdout);

    return res;
}

/* Creates the file and opens it for output on the designated file descriptor
 *
 * If the file does not exist, it shall be created; otherwise,
 * it shall be truncated to be an empty file after being opened
 */
static int eval_redir_great(struct ast_redir *ast_redir)
{
    int fd = open(ast_redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        return 1;
    }

    /* Saving the fd which will be overwritten */

    int fdout = dup(ast_redir->fd);
    if (fdout == -1)
    {
        close(fd);

        return 1;
    }

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

    int res;
    if (ast_redir->right == NULL)
    {
        res = eval(ast_redir->left);
    }
    else
    {
        res = eval(ast_redir->right);
    }

    /* Putting back the overwritten fd as it was */

    if (dup2(fdout, ast_redir->fd) == -1)
    {
        close(fdtemp);
        return 1;
    }

    close(fdout);

    return res;
}

/* Opens the file for both reading and writing on the file descriptor denoted by
 * n If the file does not exist, it shall be created
 */
static int eval_redir_lessgreat(struct ast_redir *ast_redir)

{
    int fd = open(ast_redir->file, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        return 1;
    }

    /* Saving the fd which will be overwritten */

    int fdout = dup(ast_redir->fd);
    if (fdout == -1)
    {
        close(fd);

        return 1;
    }

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

    int res;
    if (ast_redir->right == NULL)
    {
        res = eval(ast_redir->left);
    }
    else
    {
        res = eval(ast_redir->right);
    }

    /* Putting back the overwritten fd as it was */

    if (dup2(fdout, ast_redir->fd) == -1)
    {
        close(fdtemp);
        return 1;
    }

    close(fdout);

    return res;
}

/* Opens the file for output on the designated file descriptor
 * The file is opened with the O_APPEND flag
 * If the file does not exist, it shall be created
 */
static int eval_redir_dgreat(struct ast_redir *ast_redir)
{
    int fd = open(ast_redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        return 1;
    }

    /* Saving the fd which will be overwritten */

    int fdout = dup(ast_redir->fd);
    if (fdout == -1)
    {
        close(fd);

        return 1;
    }

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

    int res;
    if (ast_redir->right == NULL)
    {
        res = eval(ast_redir->left);
    }
    else
    {
        res = eval(ast_redir->right);
    }

    /* Putting back the overwritten fd as it was */

    if (dup2(fdout, ast_redir->fd) == -1)
    {
        close(fdtemp);
        return 1;
    }

    close(fdout);

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

    /* Gets the value of the source fd from file */

    long fdsrc = strtol(ast_redir->file, NULL, 10);
    if (fdsrc == LONG_MIN)
    {
        errx(1, "source file descriptor is weird");
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

    int res;
    if (ast_redir->right == NULL)
    {
        res = eval(ast_redir->left);
    }
    else
    {
        res = eval(ast_redir->right);
    }

    /* Putting back the overwritten fd as it was */

    if (dup2(fdout, ast_redir->fd) == -1)
    {
        close(fdtemp);
        return 1;
    }

    close(fdout);

    return res;
}

int eval_redir(struct ast *ast)
{
    struct ast_redir *ast_redir = &ast->data.ast_redir;

    if (ast_redir->type == LESS)
    {
        return eval_redir_less(ast_redir);
    }
    else if (ast_redir->type == GREAT || ast_redir->type == CLOBBER)
    {
        return eval_redir_great(ast_redir);
    }
    else if (ast_redir->type == LESSAND || ast_redir->type == GREATAND)
    {
        return eval_redir_anyand(ast_redir);
    }
    else if (ast_redir->type == LESSGREAT)
    {
        return eval_redir_lessgreat(ast_redir);
    }
    else if (ast_redir->type == DGREAT)
    {
        return eval_redir_dgreat(ast_redir);
    }
    else
    {
        errx(1, "invalid redir node");
    }
}
