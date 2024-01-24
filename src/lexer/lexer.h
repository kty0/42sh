#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "token.h"

struct lexer
{
    FILE *file;
    struct token current_tok; // The current free_token
    struct token next_tok;
    char charac;
};

// initialize the lexer

void concat_quoted_str(char **str, char *str_sub, int *len_tot);
struct lexer *lexer_new(FILE *file);
struct token lexer_pop(struct lexer *lexer);
struct token lexer_peek(struct lexer *lexer);
void lexer_free(struct lexer *lexer);
void free_token(struct token tok);
struct token lexer_peek_free(struct lexer *lexer);
struct token lexer_pop_free(struct lexer *lexer);
char *get_string(struct lexer *lexer, char *c);

#endif /* !LEXER_H */
