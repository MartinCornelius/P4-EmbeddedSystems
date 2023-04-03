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
    if (node->right->type == PLUS)
    {
      printf("starting optimization\n");
      int a = ((struct astLeafInt *)node->right->left->left->left)->value;
      int b = ((struct astLeafInt *)node->right->right->left)->value;
      int result = a + b;
      struct ast *constNode = allocASTLeafInt(VAL, result);
      freeAST(node->right);
      node->right = constNode;
      printf("changed value of the expression\n");
      printf("%d + %d => %d\n", a, b, result);
    }
    if (node->right->type == MINUS)
    {
      printf("starting optimization\n");
      int a = ((struct astLeafInt *)node->right->left->left->left)->value;
      int b = ((struct astLeafInt *)node->right->right->left)->value;
      int result = a - b;
      struct ast *constNode = allocASTLeafInt(VAL, result);
      freeAST(node->right);
      node->right = constNode;
      printf("changed value of the expression\n");
      printf("%d - %d => %d\n", a, b, result);
    }
    if (node->right->type == TIMES)
    {
      printf("starting optimization\n");
      int a = ((struct astLeafInt *)node->right->left->left->left)->value;
      int b = ((struct astLeafInt *)node->right->left->right)->value;
      int result = a * b;
      struct ast *constNode = allocASTLeafInt(VAL, result);
      freeAST(node->right);
      node->right = constNode;
      printf("changed value of the expression\n");
      printf("%d * %d => %d\n", a, b, result);
    }
    if (node->right->type == DIV)
    {
      printf("starting optimization\n");
      int a = ((struct astLeafInt *)node->right->left->left->left)->value;
      int b = ((struct astLeafInt *)node->right->left->right)->value;
      int result = a / b;
      struct ast *constNode = allocASTLeafInt(VAL, result);
      freeAST(node->right);
      node->right = constNode;
      printf("changed value of the expression\n");
      printf("%d / %d => %d\n", a, b, result);
    }
  }

  constantFolding(node->left);
  constantFolding(node->right);
}

#endif
