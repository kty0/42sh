#ifndef LEXER_H
#define LEXER_H

#include "token.h"

struct lexer
{
    char *input; // The input data
    size_t pos; // The current offset inside the input data
    struct token current_tok; // The next token, if processed
};

// initialize the lexer

void concat_quoted_str(char **str, char *str_sub, int *len_tot);
struct lexer *lexer_new(char *input);
struct token lexer_pop(struct lexer *lexer);
struct token lexer_peek(struct lexer *lexer);
void lexer_free(struct lexer *lexer);
void free_token(struct token tok);
struct token lexer_peek_free(struct lexer *lexer);
struct token lexer_pop_free(struct lexer *lexer);

#endif /* !LEXER_H */
