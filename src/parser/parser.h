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

enum parser_status parse_list(struct ast **res, struct lexer *lexer);

enum parser_status parse_and_or(struct ast **res, struct lexer *lexer);

enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer);

enum parser_status parse_command(struct ast **res, struct lexer *lexer);

enum parser_status parse_shell_command(struct ast **res, struct lexer *lexer);

enum parser_status parse_rule_if(struct ast **res, struct lexer *lexer);

enum parser_status parse_compound_list(struct ast **res, struct lexer *lexer);

enum parser_status parse_else_clause(struct ast **res, struct lexer *lexer);

enum parser_status parse_simple_command(struct ast **res, struct lexer *lexer);

enum parser_status parse_element(struct ast **res, struct lexer *lexer);

#endif /* !PARSER_H */
