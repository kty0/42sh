#include "lexer.h"

#include <ctype.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

static struct token tokens[] = {
    { TOKEN_IF, "if" },       { TOKEN_THEN, "then" },
    { TOKEN_ELIF, "elif" },   { TOKEN_ELSE, "else" },
    { TOKEN_FI, "fi" },       { TOKEN_SEMICOLON, ";" },
    { TOKEN_ERROR, "error" }, { TOKEN_NOT, "!" },
    { TOKEN_PIPE, "|" },      { TOKEN_WHILE, "while" },
    { TOKEN_DO, "do" },       { TOKEN_DONE, "done" },
    { TOKEN_UNTIL, "until" }, { TOKEN_FOR, "for" },
    { TOKEN_IN, "in" },       { TOKEN_AND_IF, "&&" },
    { TOKEN_OR_IF, "||" },    { TOKEN_NEWLINE, "\n" },
    { TOKEN_DGREAT, ">>" },   { TOKEN_LESSAND, "<&" },
    { TOKEN_GREATAND, ">&" }, { TOKEN_LESSGREAT, "<>" },
    { TOKEN_CLOBBER, ">|" },  { TOKEN_GREAT, ">" },
    { TOKEN_LESS, "<" }
};

void lexer_free(struct lexer *lexer)
{
    free(lexer);
}

static int check_solely_number(char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

static int check_lessgreat(char c)
{
    return c == '<' || c == '>';
}

/**
 * Create the token from the input string
 */
static struct token parse_input_for_tok(struct lexer *lexer)
{
    struct token new;
    char c = lexer->charac;
    char *string = get_string(lexer, &c);
    lexer->charac = c;
    int is_word = 1;
    new.value = NULL;
    if (string == NULL)
    {
        new.type = TOKEN_EOF;
    }
    else
    {
        for (size_t i = 0; i < sizeof(tokens) / sizeof(struct token); i++)
        {
            if (!strcmp(string, tokens[i].value))
            {
                new.type = tokens[i].type;
                is_word = 0;
                break;
            }
        }
        if (is_word)
        {
            new.type = TOKEN_WORD;
            new.value = string;
            if (check_solely_number(string) && check_lessgreat(c))
            {
                new.type = TOKEN_IONUMBER;
            }
        }
    }
    if (!is_word && new.type != TOKEN_ERROR)
    {
        free(string);
    }
    return new;
}

struct lexer *lexer_new(FILE *file)
{
    struct lexer *l = calloc(1, sizeof(struct lexer));
    if (!l)
    {
        return NULL;
    }
    l->file = file;
    l->current_tok.type = TOKEN_EOF;
    l->next_tok.type = TOKEN_EOF;
    l->charac = ' ';
    l->current_tok = parse_input_for_tok(l);
    l->next_tok = parse_input_for_tok(l);
    return l;
}

struct token lexer_peek(struct lexer *lexer)
{
    return lexer->current_tok;
}

struct token lexer_pop(struct lexer *lexer)
{
    struct token curr_tok = lexer->current_tok;
    lexer->current_tok = lexer->next_tok;
    lexer->next_tok = parse_input_for_tok(lexer);
    return curr_tok;
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
