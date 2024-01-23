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
    TOKEN_EOF, // end of file
    TOKEN_NOT, // !
    TOKEN_PIPE, // pipe
    TOKEN_WHILE, // while
    TOKEN_DO, // do
    TOKEN_DONE, // done
    TOKEN_UNTIL, // until
    TOKEN_FOR, // for
    TOKEN_IN, // in
    TOKEN_AND_IF, // and
    TOKEN_OR_IF, // or
    TOKEN_LESS, // <
    TOKEN_GREAT, // >
    TOKEN_DGREAT, // >>
    TOKEN_LESSAND, // <&
    TOKEN_GREATAND, // >&
    TOKEN_LESSGREAT, //<>
    TOKEN_CLOBBER, // >|
    TOKEN_IONUMBER,
};

struct token
{
    enum token_type type; // The kind of token
    char *value; // If the token is a number, its value
};

#endif /* !TOKEN_H */
