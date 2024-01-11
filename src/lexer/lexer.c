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
    while (lexer->input[lexer->pos] == ' ')
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
    char *str = malloc(len + 1);
    strncpy(str, begin + 1, len);
    str[len] = '\0';

    return str;
}

/**
 * Create a string before creating the tokens
 */
char *get_string(struct lexer *lexer)
{
    size_t len = 0;
    char *begin = lexer->input + lexer->pos;
    if (lexer->input[lexer->pos] == '\0')
    {
        return NULL;
    }
    char *str = NULL;
    char *hoppy = NULL;
    int sub = 0;
    int end = 0;
    int len_tot = 0;
    int deb = 1;
    while (lexer->input[lexer->pos] != ';' && lexer->input[lexer->pos] != '\0'
           && lexer->input[lexer->pos] != ' '
           && lexer->input[lexer->pos] != '\n')
    {
        deb = 0;
        if (sub)
        {
            size_t len_sub = strlen(hoppy);
            str = realloc(str, len_tot + len_sub);
            strncpy(str + len_tot, hoppy, len_sub);
            len_tot += len_sub;
            len = 0;
            sub = 0;
        }
        if (lexer->input[lexer->pos] == '"' || lexer->input[lexer->pos] == '\'')
        {
            str = realloc(str, len_tot + len);
            strncpy(str + len_tot, lexer->input + lexer->pos - len, len);
            len_tot += len;
            hoppy = str_quote(lexer);
            if (!hoppy)
            {
                free(str);
                free(hoppy);
                char *s = "Error";
                return s;
            }
            sub = 1;
            end = 0;
        }
        else
        {
            lexer->pos++;
            len++;
            end = 1;
        }
    }
    if (lexer->input[lexer->pos] == ';' && deb)
    {
        len++;
        lexer->pos++;
        end = 1;
    }
    if (lexer->input[lexer->pos] == '\n' && deb)
    {
        len++;
        lexer->pos++;
        end = 1;
    }

    str = realloc(str, len_tot + len + 1);
    if (end)
    {
        strncpy(str + len_tot, lexer->input + lexer->pos - len, len);
        len_tot += len;
    }
    else if (sub)
    {
        size_t len_sub = strlen(hoppy);
        str = realloc(str, len_tot + len_sub + 1);
        strncpy(str + len_tot, hoppy, len_sub);
        len_tot += len_sub;
    }
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
