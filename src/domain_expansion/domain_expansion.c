#include "domain_expansion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../lexer/token.h"

struct expand_functions
{
    enum exp_type exp_type;
    void (*expand_funs)(char **, size_t i);
};

static void expand_quote(char **str, size_t k)
{
    size_t len = strlen(str[k]);
    char *new_string = calloc(len, sizeof(char));
    size_t j = 0;

    for (size_t i = 0; str[k][i] != '\0'; i++)
    {
        if (str[k][i] != '\'')
        {
            new_string[j++] = str[k][i];
        }
    }
    free(str[k]);
    str[k] = new_string;
}

static int check_d_backslash(char c)
{
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

static void expand_dquote(char **str, size_t k)
{
    size_t len = strlen(str[k]);
    char *new_string = calloc(len, sizeof(char));
    size_t j = 0;

    for (size_t i = 0; str[k][i] != '\0'; i++)
    {
        if (str[k][i] != '"' && str[k][i] != '\\')
        {
            new_string[j++] = str[k][i];
        }
        else if (str[k][i] == '\\')
        {
            if (check_d_backslash(str[k][i + 1]))
            {
                if (str[k][i + 1] != '\0')
                {
                    new_string[j++] = str[k][i + 1];
                    i++;
                }
            }
            else
            {
                new_string[j++] = str[k][i];
            }
        }
    }
    free(str[k]);
    str[k] = new_string;
}

static void expand_backslash(char **str, size_t k)
{
    size_t len = strlen(str[k]);
    char *new_string = calloc(len, sizeof(char));
    size_t j = 0;

    for (size_t i = 0; str[k][i] != '\0'; i++)
    {
        if (str[k][i] != '\\')
        {
            new_string[j++] = str[k][i];
        }
        else
        {
            if (str[k][i + 1] != '\0')
            {
                new_string[j++] = str[k][i + 1];
                i++;
            }
        }
    }
    free(str[k]);
    str[k] = new_string;
}

static struct expand_functions expand_funs[] = { { QUOTE, expand_quote },
                                                 { DQUOTE, expand_dquote },
                                                 { BACKSLASH,
                                                   expand_backslash } };

void expand_command(struct ast *ast)
{
    struct ast_cmd *ast_cmd = &ast->data.ast_cmd;
    for (size_t i = 0; ast_cmd->args[i] != NULL; i++)
    {
        for (size_t j = 0;
             j < sizeof(expand_funs) / sizeof(struct expand_functions); j++)
        {
            if (*(ast_cmd->exps[i]) == expand_funs[j].exp_type)
            {
                expand_funs[j].expand_funs(ast_cmd->args, i);
            }
        }
    }
}
