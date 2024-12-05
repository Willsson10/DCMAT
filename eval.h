#ifndef __EVAL_H__
#define __EVAL_H__

#include "ast.h"

void rpn_eval(NODE_DEFAULT *node);
Result* eval(NODE_DEFAULT *node);

NODE_DEFAULT* copy_tree(NODE_DEFAULT* node);
#endif
