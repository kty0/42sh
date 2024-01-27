#ifndef EVAL_H
#define EVAL_H

#include "../ast/ast.h"

enum side
{
    LEFT,
    RIGHT
};

int eval(struct ast *ast);

#endif /* !EVAL_H */
