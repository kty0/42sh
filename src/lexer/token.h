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
    TOKEN_WORDS, // words
    TOKEN_ERROR // error
};

struct token
{
    enum token_type type; // The kind of token
    char *str;

};
#endif /* !TOKEN_H */
