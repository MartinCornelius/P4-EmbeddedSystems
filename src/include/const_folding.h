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
    printf("%s\n", printType(node->right->type));
    int type = node->right->type;
    switch (type)
    {
      case PLUS:
      case MINUS:
      case TIMES:
      case DIV:
        printf("starting optimization.\n");

        int a, b, result;

        // If only integers
        if (node->right->left->type == VAL && node->right->right->type == VAL)  
        {
          a = ((struct astLeafInt *)node->right->left)->value;
          b = ((struct astLeafInt *)node->right->right)->value;
        }

        // Do operation
        if (type == PLUS)
          result = a + b;
        else if (type == MINUS)
          result = a - b;
        else if (type == TIMES)
          result = a * b;
        else if (type == DIV)
          result = a / b;

        struct ast *newNode = allocASTLeafInt(VAL, result);
        freeAST(node->right);
        node->right = newNode;
        printf("changed value of the expression\n");
        printf("%d + %d => %d\n", a, b, result);
        break;
    }
  }

  constantFolding(node->left);
  constantFolding(node->right);
}

#endif
