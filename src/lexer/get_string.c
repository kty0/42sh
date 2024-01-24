#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static struct token tokens[] = {
    { TOKEN_SEMICOLON, ";" }, { TOKEN_NOT, "!" },
    { TOKEN_PIPE, "|" },      { TOKEN_AND_IF, "&&" },
    { TOKEN_OR_IF, "||" },    { TOKEN_NEWLINE, "\n" },
    { TOKEN_DGREAT, ">>" },   { TOKEN_LESSAND, "<&" },
    { TOKEN_GREATAND, ">&" }, { TOKEN_LESSGREAT, "<>" },
    { TOKEN_CLOBBER, ">|" },  { TOKEN_GREAT, ">" },
    { TOKEN_LESS, "<" }
};

static int check_quote(char c)
{
    return c == '\\' || c == '\'' || c == '"';
}

static int check_operator(char c)
{
    return c == ';' || c == '|' || c == '&' || c == '\n' || c == '<'
        || c == '>';
}

void append_char(char **str, int *len, char c)
{
    *str = realloc(*str, *len + 1);
    (*str)[(*len)++] = c;
}

static int existing_operator(char *str, char c, int len)
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

static int simple_quote(char **str, char *c, struct lexer *lexer, int *len)
{
    *c = fgetc(lexer->file);
    while (*c != '\'')
    {
        if (*c == EOF)
        {
            return 0;
        }
        append_char(str, len, *c);
        *c = fgetc(lexer->file);
    }
    return 1;
}

static int double_quote(char **str, char *c, struct lexer *lexer, int *len)
{
    // to be reviewed during the implementation of variables
    *c = fgetc(lexer->file);
    while (*c != '"')
    {
        if (*c == EOF)
        {
            return 0;
        }
        append_char(str, len, *c);
        *c = fgetc(lexer->file);
    }
    return 1;
}

static int backslash(struct lexer *lexer, char *c, char **str, int *len)
{
    *c = fgetc(lexer->file);
    if (*c == '\n')
    {
        *c = fgetc(lexer->file);
    }
    if (*c == EOF)
    {
        return 0;
    }
    if (!isblank(*c))
    {
        append_char(str, len, *c);
    }
    return 1;
}

static int rule_4(char **str, char *c, struct lexer *lexer, int *len)
{
    if (*c == '\'')
    {
        return simple_quote(str, c, lexer, len);
    }
    else if (*c == '\\')
    {
        return backslash(lexer, c, str, len);
    }
    return double_quote(str, c, lexer, len);
}

static int rule_9(struct lexer *lexer, char *c)
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
    int len = 0;
    while (*c != EOF)
    {
        if (len && check_operator(str[len - 1])
            && existing_operator(str, *c, len)) // rule 2
        {
            append_char(&str, &len, *c);
        }
        else if (len && check_operator(str[len - 1])
                 && !existing_operator(str, *c, len)) // rule 3
        {
            break;
        }
        else if (check_quote(*c)) // rule 4
        {
            if (!rule_4(&str, c, lexer, &len))
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
        else if (*c == '#')
        {
            if (!rule_9(lexer, c))
            {
                return free_error(str);
            }
        }
        else // rule 8 & 10
        {
            append_char(&str, &len, *c);
        }
        *c = fgetc(lexer->file);
    }
    if (*c == EOF && !len)
    {
        free(str);
        return NULL;
    }
    append_char(&str, &len, '\0');
    return str;
}
