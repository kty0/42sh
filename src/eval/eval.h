#ifndef EVAL_H
#define EVAL_H

#include "../ast/ast.h"
#include "../hash_map/hash_map.h"

enum side
{
    LEFT,
    RIGHT
};

int eval(struct ast *ast, struct hash_map *h);

int eval_redir(struct ast *ast, struct ast_cmd *ast_cmd);

int launch_command(struct ast_cmd *ast_cmd);

#endif /* !EVAL_H */
