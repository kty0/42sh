#ifndef AST_PRINT_H
#define AST_PRINT_H

#include "ast.h"

void print_if(struct ast *ast);

void print_command(struct ast *ast);

void print_list(struct ast *ast);

void print_pipe(struct ast *ast);

void print_until(struct ast *ast);

void print_while(struct ast *ast);

void print_ope(struct ast *ast);

void print_redir(struct ast *ast);

void print_for(struct ast *ast);

void ast_print(struct ast *ast);

#endif /* !AST_PRINT_H */
