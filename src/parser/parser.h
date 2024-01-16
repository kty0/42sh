#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"

enum parser_status
{
    P_OK,
    P_KO,
};

enum parser_status parse(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_H */
