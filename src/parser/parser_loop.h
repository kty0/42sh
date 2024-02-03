#ifndef PARSER_LOOP_H
#define PARSER_LOOP_H

#include "parser.h"

enum parser_status parse_rule_while(struct ast **res, struct lexer *lexer);

enum parser_status parse_rule_until(struct ast **res, struct lexer *lexer);

enum parser_status parse_rule_for(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_LOOP_H */
