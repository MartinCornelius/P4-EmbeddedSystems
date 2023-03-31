#ifndef AST_H
#define AST_H

#include "../parser.tab.h"

struct ast
{
  int type;
  struct ast *left;
  struct ast *right;
};

struct astLeaf
{
  int type;
  int value;
};

struct astLeafStr
{
  int type;
  char *string;
};

/* Build AST */
struct ast *allocAST(int type, struct ast *left, struct ast *right)
{
  struct ast *node = malloc(sizeof(struct ast));
  if (!node)
  {
    printf("out of space\n");
    exit(0);
  }
  node->type = type;
  node->left = left;
  node->right = right;
  return node;
}

struct ast *allocASTLeafInt(int type, int value)
{
  struct astLeaf *node = malloc(sizeof(struct astLeaf));
  if (!node)
  {
    printf("out of space\n");
    exit(0);
  }
  node->type = type;
  node->value = value;
  return (struct ast *)node;
}

struct ast *allocASTLeafStr(int type, char *string)
{
  struct astLeafStr *node = malloc(sizeof(struct astLeafStr));
  if (!node)
  {
    printf("out of space\n");
    exit(0);
  }
  node->type = type;
  node->string = string;
  return (struct ast *)node;
}

/* Evaluate AST */
int evalAST(struct ast *node)
{
  int result;

  switch (node->type)
  {
  case PLUS:
    result = evalAST(node->left) + evalAST(node->right);
    break;
  case VAL:
    result = ((struct astLeaf *)node)->value;
    break;
  default:
    printf("internal error: bad node\n");
  }

  return result;
}

void printAST(struct ast *node)
{
  switch (node->type)
  {
  case PLUS:
    printAST(node->left);
    printf(" PLUS ");
    printAST(node->right);
    break;
  case VAL:
    printf("VAL: %d", ((struct astLeaf *)node)->value);
    break;
  }
}

/* Free memory */
void freeAST(struct ast *node)
{
  switch (node->type)
  {
  case PLUS:
    freeAST(node->left);
  case VAL:
    free(node);
    break;
  default:
    printf("internal error: free bad node\n");
  }
}

#endif