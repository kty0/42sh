#ifndef PARSER_COMMAND_H
#define PARSER_COMMAND_H

#include "parser.h"

struct cmd_pack
{
    struct ast **node;
    struct ast **result;
};

enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer);

enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer);

enum parser_status parse_command(struct ast **res, struct lexer *lexer);

enum parser_status parse_funcdec(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_COMMAND_H */
