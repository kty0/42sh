#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

static struct token tokens[] = {
    { TOKEN_SEMICOLON, ";" }, { TOKEN_NOT, "!" },
    { TOKEN_PIPE, "|" },      { TOKEN_AND_IF, "&&" },
    { TOKEN_OR_IF, "||" },    { TOKEN_NEWLINE, "\n" },
    { TOKEN_DGREAT, ">>" },   { TOKEN_LESSAND, "<&" },
    { TOKEN_GREATAND, ">&" }, { TOKEN_LESSGREAT, "<>" },
    { TOKEN_CLOBBER, ">|" },  { TOKEN_GREAT, ">" },
    { TOKEN_LESS, "<" }
};

static int len = 0;

static int check_quote(char c)
{
    return c == '\\' || c == '\'' || c == '"';
}

static int check_operator(char c)
{
    return c == ';' || c == '|' || c == '&' || c == '\n' || c == '<'
        || c == '>';
}

void append_char(char **str, char c)
{
    *str = realloc(*str, len + 1);
    (*str)[len++] = c;
}

static int existing_operator(char *str, char c)
{
    char *op = malloc((len + 2) * sizeof(char));
    strncpy(op, str, len);
    op[len] = c;
    op[len + 1] = '\0';
    for (size_t i = 0; i < sizeof(tokens) / sizeof(struct token); i++)
    {
        if (!strcmp(op, tokens[i].value))
        {
            free(op);
            return 1;
        }
    }
    free(op);
    return 0;
}

static int simple_quote(char **str, char *c, struct lexer *lexer)
{
    append_char(str, *c);
    *c = fgetc(lexer->file);
    while (*c != '\'')
    {
        if (*c == EOF)
        {
            return 0;
        }
        append_char(str, *c);
        *c = fgetc(lexer->file);
    }
    append_char(str, *c);
    return 1;
}

static int double_quote(char **str, char *c, struct lexer *lexer)
{
    // to be reviewed during the implementation of variables
    append_char(str, *c);
    *c = fgetc(lexer->file);
    while (*c != '"')
    {
        if (*c == EOF)
        {
            return 0;
        }
        append_char(str, *c);
        *c = fgetc(lexer->file);
    }
    append_char(str, *c);
    return 1;
}

static int backslash(struct lexer *lexer, char *c, char **str)
{
    char bs = *c;
    *c = fgetc(lexer->file);
    if (*c == EOF)
    {
        return 0;
    }
    if (*c != '\n')
    {
        append_char(str, bs);
        append_char(str, *c);
        return 1;
    }
    return 2;
}

static int quoting_handler(char **str, char *c, struct lexer *lexer)
{
    if (*c == '\'')
    {
        return simple_quote(str, c, lexer);
    }
    else if (*c == '\\')
    {
        return backslash(lexer, c, str);
    }
    return double_quote(str, c, lexer);
}

static int comments_skip(struct lexer *lexer, char *c)
{
    while (*c != '\n' && *c != EOF)
    {
        *c = fgetc(lexer->file);
    }
    return 1;
}

static char *free_error(char *str)
{
    free(str);
    return "error";
}

char *get_string(struct lexer *lexer, char *c)
{
    // if segfault check append_char
    char *str = NULL;
    len = 0;
    while (*c != EOF)
    {
        if (len && check_operator(str[len - 1])
            && existing_operator(str, *c)) // rule 2
        {
            append_char(&str, *c);
        }
        else if (len && check_operator(str[len - 1])
                 && !existing_operator(str, *c)) // rule 3
        {
            break;
        }
        else if (check_quote(*c)) // rule 4
        {
            if (!quoting_handler(&str, c, lexer))
            {
                return free_error(str);
            }
        }
        else if (len && check_operator(*c)) // rule 6
        {
            break;
        }
        else if (isblank(*c)) // rule 7
        {
            if (len)
            {
                break;
            }
        }
        else if (*c == '#' && !len)
        {
            if (!comments_skip(lexer, c))
            {
                return free_error(str);
            }
        }
        else // rule 8 & 10
        {
            append_char(&str, *c);
        }
        *c = fgetc(lexer->file);
    }
    if (*c == EOF && !len)
    {
        free(str);
        return NULL;
    }
    append_char(&str, '\0');
    return str;
}
