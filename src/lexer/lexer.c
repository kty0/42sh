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

void lexer_skip_whitespace(struct lexer *lexer)
{
    while (lexer->input[lexer->pos] == ' ' || lexer->input[lexer->pos] == '\t')
    {
        lexer->pos++;
    }
}

void lexer_skip_comments(struct lexer *lexer)
{
    while (lexer->input[lexer->pos] != '\n' && lexer->input[lexer->pos] != '\0')
    {
        lexer->pos++;
    }
}

/**
 * Create a string that handles the quote and double quote case
 */
char *str_quote(struct lexer *lexer)
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
        return NULL;
    }
    char *str = malloc(len);
    strncpy(str, begin + 1, len);

    return str;
}

/**
 * check is the caracter is a delimiter or not
 */
size_t is_delim(char c)
{
    return c == ';' || c == '\0' || c == ' ' || c == '\n';
}

/**
 * concatenate a quoted string with a string
 * use after generating a string from a quoted string
 */
void concat_quoted_str(char **str, char *str_sub, int *len_tot)
{
    size_t len_sub = strlen(str_sub);
    *str = realloc(*str, *len_tot + len_sub);
    strncpy(*str + *len_tot, str_sub, len_sub + 1);
    *len_tot += len_sub;
}

/**
 * concatenate two substring
 * use before generating a string from a quoted string
 */
void concat_str(char **str, char *str_sub, int *len_tot, int len)
{
    *str = realloc(*str, *len_tot + len);
    strncpy(*str + *len_tot, str_sub, len);
    *len_tot += len;
}

/**
 * if the current caractere is a '#' then check if it is a comment or not
 */
int check_comment(struct lexer *lexer)
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
void check_semicolon_and_backslah_n(struct lexer *lexer, size_t *len, int deb)
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
char *get_string(struct lexer *lexer)
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
    char *hoppy = NULL;
    int sub = 0;
    int len_tot = 0;
    int deb = 1;
    while (!is_delim(lexer->input[lexer->pos]))
    {
        if (check_comment(lexer))
            break;
        deb = 0;
        if (sub)
        {
            concat_quoted_str(&str, hoppy, &len_tot);
            len = 0;
            sub = 0;
        }
        if (lexer->input[lexer->pos] == '"' || lexer->input[lexer->pos] == '\'')
        {
            concat_str(&str, lexer->input + lexer->pos - len, &len_tot, len);
            hoppy = str_quote(lexer);
            if (!hoppy)
            {
                free(str);
                free(hoppy);
                char *s = "Error";
                return s;
            }
            sub = 1;
        }
        else
        {
            lexer->pos++;
            len++;
            sub = 0;
        }
    }
    check_semicolon_and_backslah_n(lexer, &len, deb);
    if (!sub)
    {
        concat_str(&str, lexer->input + lexer->pos - len, &len_tot, len);
    }
    else if (sub)
    {
        concat_quoted_str(&str, hoppy, &len_tot);
    }
    str = realloc(str, len_tot + 1);
    str[len_tot] = '\0';

    return str;
}

/**
 * Create the token from the input string
 */
struct token parse_input_for_tok(struct lexer *lexer)
{
    struct token new;
    lexer_skip_whitespace(lexer);
    char *string = get_string(lexer);
    int to_free = 1;
    new.str = NULL;
    if (string == NULL)
    {
        new.type = TOKEN_EOF;
    }
    else if (string[0] == '\n')
    {
        new.type = TOKEN_NEWLINE;
        to_free = 0;
    }
    else if (strcmp(string, "Error") == 0)
    {
        new.type = TOKEN_ERROR;
        to_free = 0;
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
        new.type = TOKEN_WORDS;
        new.str = string;
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
    if (lexer->pos == 0 && lexer->current_tok.type == TOKEN_ERROR)
    {
        lexer->current_tok = parse_input_for_tok(lexer);
    }
    struct token curr_tok = lexer->current_tok;
    lexer->current_tok = parse_input_for_tok(lexer);
    return curr_tok;
}

void free_token(struct token tok)
{
    if (tok.str)
        free(tok.str);
}
