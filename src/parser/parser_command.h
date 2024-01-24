#ifndef PARSER_COMMAND_H
#define PARSER_COMMAND_H

#include "parser.h"

enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer);

enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer);

enum parser_status parse_command(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_COMMAND_H */
