#ifndef PARSER_IO_H
#define PARSER_IO_H

#include "parser.h"

enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer);

enum parser_status parse_redirection(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_IO_H */
