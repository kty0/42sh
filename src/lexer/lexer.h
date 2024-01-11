#ifndef LEXER_H
#define LEXER_H

#include "token.h"

struct lexer
{
    char *input; // The input data
    size_t pos; // The current offset inside the input data
    struct token current_tok; // The next token, if processed
};

//initialize the lexer
struct lexer *lexer_new(char *input);
struct token lexer_pop(struct lexer *lexer);
struct token lexer_peek(struct lexer *lexer);
struct token parse_input_for_tok(struct lexer *lexer);
char *get_string(struct lexer *lexer);
char *str_quote(struct lexer *lexer);
void lexer_skip_whitespace(struct lexer *lexer);
void lexer_free(struct lexer *lexer);
void free_token(struct token tok);

#endif /* !LEXER_H */
