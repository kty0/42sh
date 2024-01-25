#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "eval.h"
#include "../ast/ast.h"
#include "../built_in/echo.h"
#include "../built_in/true_false.h"

static int eval_redir_less(struct ast_redir *ast_redir)
{
    int fd = open(ast_redir->file, O_RDONLY);
    if (fd == -1)
    {
        return 1;
    }

    off_t length = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);

    char *buffer = malloc(sizeof(char) * (length + 1));

    if (read(fd, buffer, length) == -1)
    {
        errx(1, "reading failed in redir less");
    }

    if (write(ast_redir->fd, buffer, length) == -1)
    {
        errx(1, "writing failed in redir less");
    }

    free(buffer);
    close(fd);

    return 0;
}

static int eval_redir_great(struct ast_redir *ast_redir)
{
    int fd = open(ast_redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        return 1;
    }

    int fdout = dup(ast_redir->fd);
    if (fdout == -1)
    {
        close(fd);

        return 1;
    }

    int fdtemp = dup2(fd, ast_redir->fd);
    if (fdtemp == -1)
    {
        close(fd);
        close(fdout);

        return 1;
    }

    close(fd);

    int res;
    if (ast_redir->right == NULL)
    {
        res = eval(ast_redir->left);
    }
    else
    {
        res = eval(ast_redir->right);
    }

    if (dup2(fdout, ast_redir->fd) == -1)
    {
        close(fdtemp);
        return 1;
    }

    close(fdout);

    return res;
}

static int eval_redir_lessgreat(struct ast_redir *ast_redir)
{
    int fd = open(ast_redir->file, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        return 1;
    }

    int fdout = dup(ast_redir->fd);
    if (fdout == -1)
    {
        close(fd);

        return 1;
    }

    int fdtemp = dup2(fd, ast_redir->fd);
    if (fdtemp == -1)
    {
        close(fd);
        close(fdout);

        return 1;
    }

    close(fd);

    int res;
    if (ast_redir->right == NULL)
    {
        res = eval(ast_redir->left);
    }
    else
    {
        res = eval(ast_redir->right);
    }

    if (dup2(fdout, ast_redir->fd) == -1)
    {
        close(fdtemp);
        return 1;
    }

    close(fdout);

    return res;
}

static int eval_redir_dgreat(struct ast_redir *ast_redir)
{
    int fd = open(ast_redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        return 1;
    }

    int fdout = dup(ast_redir->fd);
    if (fdout == -1)
    {
        close(fd);

        return 1;
    }

    int fdtemp = dup2(fd, ast_redir->fd);
    if (fdtemp == -1)
    {
        close(fd);
        close(fdout);

        return 1;
    }

    close(fd);

    int res;
    if (ast_redir->right == NULL)
    {
        res = eval(ast_redir->left);
    }
    else
    {
        res = eval(ast_redir->right);
    }

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
    else if (ast_redir->type == GREAT)
    {
        return eval_redir_greater(ast_redir);
    }
    else if (ast_redir->type == LESSAND)
    {
        return eval_redir_lessand(ast_redir);
    }
    else if (ast_redir->type == GREATAND)
    {
        return eval_redir_greatand(ast_redir);
    }
    else if (ast_redir->type == LESSGREAT)
    {
        return eval_redir_lessgreat(ast_redir);
    }
    else if (ast_redir->type == DGREAT)
    {
        return eval_redir_dgreat(ast_redir);
    }
    else if (ast_redir->type == CLOBBER)
    {
        return eval_redir_clobber(ast_redir);
    }
    else
    {
        errx(1, "invalid redir node");
    }
}
