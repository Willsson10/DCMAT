#ifndef __EVAL_H__
#define __EVAL_H__

#include "ast.h"

void rpn_eval(AST_NODE *node);
Result* eval(AST_NODE *node);

AST_NODE* copy_tree(AST_NODE* node);
#endif
