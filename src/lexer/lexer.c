#include "lexer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

struct lexer *lexer_new(char *input)
{
    struct lexer *l = malloc(sizeof(struct lexer));
    if (!l)
    {
        return NULL;
    }
    l->input = input;
    l->pos = 0;
    l->current_tok.type = TOKEN_ERROR;
    return l;
}

void lexer_free(struct lexer *lexer)
{
    free(lexer);
}

static void lexer_skip_whitespace(struct lexer *lexer)
{
    while (lexer->input[lexer->pos] == ' ' || lexer->input[lexer->pos] == '\t')
    {
        lexer->pos++;
    }
}

static void lexer_skip_comments(struct lexer *lexer)
{
    while (lexer->input[lexer->pos] != '\n' && lexer->input[lexer->pos] != '\0')
    {
        lexer->pos++;
    }
}

/**
 * concatenate a quoted string with a string
 * use after generating a string from a quoted string
 */
void concat_quoted_str(char **str, char *str_sub, int *len_tot)
{
    size_t len_sub = strlen(str_sub) + 1;
    *str = realloc(*str, *len_tot + len_sub);
    strncpy(*str + *len_tot, str_sub, len_sub);
    len_sub--;
    *len_tot += len_sub;
}

/**
 * generate a string fromm the quoted string and concatenate it into the full
 * string
 */
static void str_quote(struct lexer *lexer, char **str, int *len_tot)
{
    char *begin = lexer->input + lexer->pos++;
    int len = 0;
    int check_end = 0;

    while (check_end == 0 && lexer->input[lexer->pos] != '\0')
    {
        if (lexer->input[lexer->pos] == begin[0])
        {
            check_end = 1;
            lexer->pos++;
        }
        else
        {
            lexer->pos++;
            len++;
        }
    }
    if (lexer->input[lexer->pos] == '\0' && check_end == 0)
    {
        free(*str);
        *str = NULL;
        return;
    }
    char *str_sub = malloc(len + 1);
    strncpy(str_sub, begin + 1, len);
    str_sub[len] = '\0';
    concat_quoted_str(&(*str), str_sub, &(*len_tot));
    free(str_sub);
}

/**
 * check is the caracter is a delimiter or not
 */
static size_t is_delim(char c)
{
    return c == ';' || c == '\0' || c == ' ' || c == '\n';
}

/**
 * concatenate two substring
 * use before generating a string from a quoted string
 */
static void concat_str(char **str, char *str_sub, int *len_tot, int len)
{
    *str = realloc(*str, *len_tot + len);
    strncpy(*str + *len_tot, str_sub, len);
    *len_tot += len;
}

/**
 * if the current caractere is a '#' then check if it is a comment or not
 */
static int check_comment(struct lexer *lexer)
{
    if (lexer->input[lexer->pos] == '#')
    {
        if (lexer->input[lexer->pos - 1] != '\\')
            return 1;
    }
    return 0;
}

/**
 * Check if its is a semicolon or a backslash n to generate a string ";" or "\n"
 */
static void check_semicolon_and_backslah_n(struct lexer *lexer, size_t *len,
                                           int deb)
{
    if ((lexer->input[lexer->pos] == ';' || lexer->input[lexer->pos] == '\n')
        && deb)
    {
        (*len)++;
        lexer->pos++;
    }
}

/**
 * Create a string to creatw the tokens
 */
static char *get_string(struct lexer *lexer)
{
    size_t len = 0;
    if (lexer->input[lexer->pos] == '\0')
    {
        return NULL;
    }
    if (lexer->input[lexer->pos] == '#')
    {
        lexer_skip_comments(lexer);
    }
    char *str = NULL;
    int len_tot = 0;
    int deb = 1;
    while (!is_delim(lexer->input[lexer->pos]))
    {
        if (check_comment(lexer))
            break;
        deb = 0;
        if (lexer->input[lexer->pos] == '"' || lexer->input[lexer->pos] == '\'')
        {
            concat_str(&str, lexer->input + lexer->pos - len, &len_tot, len);
            str_quote(lexer, &str, &len_tot);
            if (!str)
            {
                char *s = "Error";
                return s;
            }
            len = 0;
        }
        else
        {
            lexer->pos++;
            len++;
        }
    }
    check_semicolon_and_backslah_n(lexer, &len, deb);
    concat_str(&str, lexer->input + lexer->pos - len, &len_tot, len);
    str = realloc(str, len_tot + 1);
    str[len_tot] = '\0';

    return str;
}

/**
 * Create the token from the input string
 */
static struct token parse_input_for_tok(struct lexer *lexer)
{
    struct token new;
    lexer_skip_whitespace(lexer);
    char *string = get_string(lexer);
    int to_free = 1;
    new.value = NULL;
    if (string == NULL)
    {
        new.type = TOKEN_EOF;
    }
    else if (string[0] == '\n')
    {
        new.type = TOKEN_NEWLINE;
    }
    else if (strcmp(string, "Error") == 0)
    {
        new.type = TOKEN_ERROR;
    }
    else if (strcmp(string, "if") == 0)
    {
        new.type = TOKEN_IF;
    }
    else if (strcmp(string, "then") == 0)
    {
        new.type = TOKEN_THEN;
    }
    else if (strcmp(string, "elif") == 0)
    {
        new.type = TOKEN_ELIF;
    }
    else if (strcmp(string, "else") == 0)
    {
        new.type = TOKEN_ELSE;
    }
    else if (strcmp(string, "fi") == 0)
    {
        new.type = TOKEN_FI;
    }
    else if (strcmp(string, ";") == 0)
    {
        new.type = TOKEN_SEMICOLON;
    }
    else
    {
        new.type = TOKEN_WORD;
        new.value = string;
        to_free = 0;
    }
    if (to_free)
        free(string);
    return new;
}

struct token lexer_peek(struct lexer *lexer)
{
    int pos = lexer->pos;
    struct token initial_tok = lexer->current_tok;
    struct token res = lexer_pop(lexer);
    lexer->pos = pos;
    lexer->current_tok = initial_tok;
    return res;
}

struct token lexer_pop(struct lexer *lexer)
{
    struct token curr_tok;
    lexer->current_tok = parse_input_for_tok(lexer);
    curr_tok = lexer->current_tok;
    return curr_tok;
}

struct token lexer_peek_free(struct lexer *lexer)
{
    struct token res = lexer_peek(lexer);
    free_token(res);
    return res;
}

struct token lexer_pop_free(struct lexer *lexer)
{
    struct token res = lexer_pop(lexer);
    free_token(res);
    return res;
}

void free_token(struct token tok)
{
    if (tok.value)
    {
        free(tok.value);
    }

    tok.value = NULL;
}
