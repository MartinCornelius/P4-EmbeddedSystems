#ifndef CONST_FOLDING_H
#define CONST_FOLDING_H

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "type2text.h"

void constantFolding(struct ast *node)
{
  if (node == NULL || node->type == VAL || node->type == VALF || node->type == ID || node->type == EMPTY)
    return;

  printf("current node %s\n", printType(node->type));

  if (node->type == ASSIGN)
  {
    int result = evaluateAST(node->right);
    struct ast *newNode = allocASTLeafInt(VAL, result);
    freeAST(node->right);
    node->right = newNode;
    printf("changed value of expression\n");
  }

  constantFolding(node->left);
  constantFolding(node->right);
}

#endif
