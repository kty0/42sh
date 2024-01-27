#ifndef PARSER_MISC_H
#define PARSER_MISC_H

#include "parser.h"

enum parser_status parse_element(struct ast **res, struct lexer *lexer);

enum parser_status parse_compound_list(struct ast **res, struct lexer *lexer);

enum parser_status parse_and_or(struct ast **res, struct lexer *lexer);

enum parser_status parse_list(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_MISC_H */
