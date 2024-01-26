#ifndef AST_NEW_H
#define AST_NEW_H

#include "../lexer/token.h"
#include "ast.h"
#include "../lexer/token.h"

struct ast *ast_new(enum ast_type type);

struct ast *ast_new_cmd(void);

struct ast *ast_new_if(void);

struct ast *ast_new_list(void);

struct ast *ast_new_not(void);

struct ast *ast_new_pipe(void);

struct ast *ast_new_while(void);

struct ast *ast_new_until(void);

struct ast *ast_new_ope(void);

struct ast *ast_new_redir(enum token_type type);

struct ast *ast_new_word(void);

#endif /* AST_NEW_H */
