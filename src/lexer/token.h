#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    TOKEN_IF, // "if"
    TOKEN_THEN, // "then"
    TOKEN_ELIF, // "elif"
    TOKEN_ELSE, // "else"
    TOKEN_FI, // "fi"
    TOKEN_SEMICOLON, // ";"
    TOKEN_NEWLINE, // "\n"
    TOKEN_WORD, // words
    TOKEN_ERROR, // error
    TOKEN_EOF // end of file
};

struct token
{
    enum token_type type; // The kind of token
    char *value; // If the token is a number, its value
};

#endif /* !TOKEN_H */
