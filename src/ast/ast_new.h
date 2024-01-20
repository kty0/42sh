#ifndef AST_NEW_H
#define AST_NEW_H

#include "ast.h"

struct ast *ast_new(enum ast_type type);

struct ast *ast_new_cmd(void);

struct ast *ast_new_if(void);

struct ast *ast_new_list(void);

struct ast *ast_new_not(void);

struct ast *ast_new_pipe(void);

struct ast *ast_new_while(void);

struct ast *ast_new_until(void);

#endif /* !AST_NEW_H */
