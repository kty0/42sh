#ifndef PARSER_IF_H
#define PARSER_IF_H

#include "parser.h"

enum parser_status parse_rule_if(struct ast **res, struct lexer *lexer);
enum parser_status parse_else_clause(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_IF_H */
