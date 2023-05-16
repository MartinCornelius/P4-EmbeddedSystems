#ifndef CONST_FOLDING_H
#define CONST_FOLDING_H

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "type2text.h"

int applicable;

void containsVar(struct ast *node);

void constantFolding(struct ast *node)
{
  if (node == NULL || node->type == VAL || node->type == VALF || node->type == ID || node->type == EMPTY)
    return;

  printf("current node %s\n", printType(node->type));

  if (node->type == ASSIGN)
  {
    applicable = 1;
    containsVar(node->right);
    if(applicable)
    {
      int result = evaluateAST(node->right);
      struct ast *newNode = allocASTLeafInt(VAL, result);
      freeAST(node->right);
      node->right = newNode;
      printf("changed value of expression\n");
    }
  }
  constantFolding(node->left);
  constantFolding(node->right);
}

void containsVar(struct ast *node){
  if (node == NULL || node->type == EMPTY || node->type == VAL || node->type == VALF){
    return;
  }

  if (node->type == ID){
    applicable = 0;
    return;
  }

  if(node->right != NULL)
    containsVar(node->right);

  if(node->left != NULL)
    containsVar(node->left);
  return;
}

#endif
