#ifndef AST_H
#define AST_H

#include "../parser.tab.h"
#include "type2text.h"

struct ast
{
  int type;
  struct ast *left;
  struct ast *right;
};

struct astIfNode
{
  int type;
  struct ast *left;
  struct ast *middle;
  struct ast *right;
};

struct astLeafInt
{
  int type;
  int value;
};

struct astLeafFloat
{
  int type;
  float value;
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
    printf("AST node failed to allocate. Out of space\n");
    exit(0);
  }
  node->type = type;
  node->left = left;
  node->right = right;
  return node;
}

struct ast *allocASTIfNode(int type, struct ast *left, struct ast *middle, struct ast *right)
{
  struct astIfNode *node = malloc(sizeof(struct astIfNode));
  if (!node)
  {
    printf("Ast if node failed to allocate. Out of space\n");
    exit(0);
  }
  node->type = type;
  node->left = left;
  node->middle = middle;
  node->right = right;
  return (struct ast *)node;
}

struct ast *allocASTLeafInt(int type, int value)
{
  struct astLeafInt *node = malloc(sizeof(struct astLeafInt));
  if (!node)
  {
    printf("AST Leaf int failed to allocate. Out of space\n");
    exit(0);
  }
  node->type = type;
  node->value = value;
  return (struct ast *)node;
}

struct ast *allocASTLeafFloat(int type, float value)
{
  struct astLeafFloat *node = malloc(sizeof(struct astLeafFloat));
  if (!node)
  {
    printf("AST Leaf float failed to allocate. Out of space\n");
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
    printf("AST Leaf string failed to allocate. Out of space\n");
    exit(0);
  }
  node->type = type;
  node->string = strdup(string);
  return (struct ast *)node;
}

void printAST(struct ast *node, int level)
{
  int leafFound = 0;

  if (node == NULL)
    return;

  printf("%*s", level * 4, ""); // Indent based on the level of the node
  printf("%s", printType(node->type));
  if (node->type == VAL)
  {
    printf(" %d", ((struct astLeafInt *)node)->value);
    leafFound = 1;
  }
  else if (node->type == VALF)
  {
    printf(" %f", ((struct astLeafFloat *)node)->value);
    leafFound = 1;
  }
  else if (node->type == ID)
  {
    printf(" %s", ((struct astLeafStr *)node)->string);
    leafFound = 1;
  }
  printf("\n");

  if (leafFound == 1)
    return;

  if (node->type == IF)
  {
    struct astIfNode *ifNode= (struct astIfNode *)node;
    printAST(ifNode->left, level + 1);
    printAST(ifNode->middle, level + 1);
    printAST(ifNode->right, level + 1);
  }
  else
  {
    printAST(node->left, level + 1);
    printAST(node->right, level + 1);
  }
}

/* Free memory */
void freeAST(struct ast *node)
{
  switch (node->type)
  {
  case SETUP:
  case MAIN:
  case CONTROL:
  case PRINT:
  case IF:
  case ELSEIF:
  case WHILE:
  case LOGOR:
  case LOGAND:
  case COPGE:
  case COPLE:
  case COPEQ:
  case COPNEQ:
  case COPG:
  case COPL:
  case ASSIGN:
  case LARROW:
  case PLUS:
  case MINUS:
  case DIV:
  case TIMES:
  case LINES:
    freeAST(node->right);
  case ELSE:
    freeAST(node->left);
  case ID:
  case VAL:
  case VALF:
  case EMPTY:
    free(node);
    break;
  default:
    printf("internal error: free bad node\n");
  }
}

#endif
