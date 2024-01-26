#ifndef PARSER_IO_H
#define PARSER_IO_H

#include "parser.h"

enum parser_status parse_pipeline(struct ast **res, struct lexer *lexer);

<<<<<<< HEAD
enum parser_status parse_redirection(struct ast **res, struct lexer *lexer);

enum parser_status parse_prefix(struct ast **res, struct lexer *lexer);

=======
>>>>>>> 1c5a7cd ([REFACTOR][PARSER] cuts down parser into smaller files)
#endif /* !PARSER_IO_H */
