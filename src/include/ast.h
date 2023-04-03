#ifndef AST_H
#define AST_H

#include "../parser.tab.h"

struct ast
{
  int type;
  struct ast *left;
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

char *typeToOut(int type)
{
  static char output[255];
  switch (type)
  {
  case LINES:
    sprintf(output, "LINES");
    break;
  case ASSIGN:
    sprintf(output, "ASSIGN");
    break;
  case ID:
    sprintf(output, "ID");
    break;
  case PLUS:
    sprintf(output, "PLUS");
    break;
  case VAL:
    sprintf(output, "VAL");
    break;
  case VALF:
    sprintf(output, "VALF");
    break;
  default:
    printf("internal error: bad node\n");
  }

  return output;
}

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

void printAST(struct ast *node)
{
  switch (node->type)
  {
  /* Main */
  case ROOT:
    printAST(node->left);
    printAST(node->right);
    break;
  case SETUP:
    printf("SETUP ");
    printAST(node->left);
    break;
  case MAIN:
    printf("MAIN ");
    printAST(node->left);
    break;
  case LINES:
    printAST(node->left);
    printf(";\n");
    printAST(node->right);
    break;

  /* Control structures */
  case CONTROL:
    printf("CONTROL ");
    printAST(node->left);
    printAST(node->right);
    break;
  case IF:
    printf("IF ");
    printAST(node->left);
    printf(" {\n");
    printAST(node->right);
    printf("}\n");
    break;
  case ELSEIF:
    printf("ELSE IF ");
    printAST(node->left);
    printf(" {\n");
    printAST(node->right);
    printf("}\n");
    break;
  case ELSE:
    printf("ELSE ");
    printAST(node->left);
    break;
  case WHILE:
    printf("WHILE ");
    printAST(node->left);
    printf(" {\n");
    printAST(node->right);
    printf("}\n");
    break;

  /* Logical operators */
  case LOGOR:
    printAST(node->left);
    printf(" LOGOR ");
    printAST(node->right);
    break;
  case LOGAND:
    printAST(node->left);
    printf(" LOGAND ");
    printAST(node->right);
    break;

  /* Compare operators */
  case COPGE:
    printAST(node->left);
    printf(" COPGE ");
    printAST(node->right);
    break;
  case COPLE:
    printAST(node->left);
    printf(" COPLE ");
    printAST(node->right);
    break;
  case COPEQ:
    printAST(node->left);
    printf(" COPEQ ");
    printAST(node->right);
    break;
  case COPNEQ:
    printAST(node->left);
    printf(" COPNEQ ");
    printAST(node->right);
    break;
  case COPG:
    printAST(node->left);
    printf(" COPG ");
    printAST(node->right);
    break;
  case COPL:
    printAST(node->left);
    printf(" COPL ");
    printAST(node->right);
    break;

  /* Operations */
  case ASSIGN:
    printAST(node->left);
    printf(" ASSIGN ");
    printAST(node->right);
    break;
  case LARROW:
    printAST(node->left);
    printf(" LARROW ");
    printAST(node->right);
    break;

  /* Arithmetic */
  case PLUS:
    printAST(node->left);
    printf(" PLUS ");
    printAST(node->right);
    break;
  case MINUS:
    printAST(node->left);
    printf(" MINUS ");
    printAST(node->right);
    break;
  case DIV:
    printAST(node->left);
    printf(" DIV ");
    printAST(node->right);
    break;
  case TIMES:
    printAST(node->left);
    printf(" TIMES ");
    printAST(node->right);
    break;

  /* Leafs */
  case ID:
    printf("ID: %s", ((struct astLeafStr *)node)->string);
    break;
  case VAL:
    printf("VAL: %d", ((struct astLeafInt *)node)->value);
    break;
  case VALF:
    printf("VALF: %f", ((struct astLeafFloat *)node)->value);
    break;
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