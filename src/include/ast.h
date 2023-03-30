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

struct ast *allocASTLeaf(int value)
{
  struct astLeaf *node = malloc(sizeof(struct astLeaf));
  if (!node)
  {
    printf("out of space\n");
    exit(0);
  }
  node->type = VAL;
  node->value = value;
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
    printf("bad node\n");
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
}