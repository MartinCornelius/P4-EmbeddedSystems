#ifndef LLVM_CODE_GEN
#define LLVM_CODE_GEN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ifCounter = 1;
int cmpCounter = 1;
int whileCounter = 1;
int tmpVarCounter = 1;
char tmpVarName[30];
char *currentVarName = "";

char variables[25][25]; // Remove me when symbol table

void addVariable(char *name)
{
  int found = 0;
  int i = 0;
  for (i = 0; i < 25; i++)
  {
    if (strcmp(name, variables[i]) == 0)
    {
      found = 1;
    }
  }
  if (found == 0)
  {
    for (i = 0; i < 25; i++)
    {
      if (strcmp(variables[i], "") == 0)
      {
        strcpy(variables[i], name);
        break;
      }
    }
  }
}

void generateLLVMCode(struct ast *node);

void generateSetup(struct ast *node)
{
  fprintf(file, "define void @setup() {\n");
  fprintf(file, "entry:\n");
  generateLLVMCode(node->left);
  fprintf(file, "\tret void\n");
  fprintf(file, "}\n\n");
}

void generateMainloop(struct ast *node)
{
  fprintf(file, "define void @mainloop() {\n");
  fprintf(file, "entry:\n");
  generateLLVMCode(node->left);
  fprintf(file, "\tret void\n");
  fprintf(file, "}\n\n");
}

void generateLLVMCode(struct ast *node)
{
  if (node == NULL)
    return;

  switch (node->type)
  {
  /* Main */
  case ROOT:
    generateLLVMCode(node->left);
    generateLLVMCode(node->right);
    break;
  case SETUP:
    generateSetup(node);
    break;
  case MAIN:
    generateMainloop(node);
    break;
  case LINES:
    generateLLVMCode(node->left);
    fprintf(file, "\n");
    generateLLVMCode(node->right);
    break;

  /* Operations */
  case ASSIGN:
    // Plus & Minus
    if (node->right->type == PLUS || node->right->type == MINUS)
    {
      fprintf(file, "\t");
      generateLLVMCode(node->left);
      fprintf(file, " = ");
      generateLLVMCode(node->right);
      fprintf(file, "\n\tstore i32 %%%s, i32* @%s", currentVarName, currentVarName);
    }
    else if (node->right->type == VAL)
    {
      /* Don't touch the following 3 lines :) */
      fseek(file, 0, SEEK_CUR);
      generateLLVMCode(node->left);
      fseek(file, -2, SEEK_CUR);
      sprintf(tmpVarName, "tmp%d", tmpVarCounter);

      fprintf(file, "\t");
      fprintf(file, "%%%s = alloca i32\n", tmpVarName);
      fprintf(file, "\tstore i32 ");
      generateLLVMCode(node->right);
      fprintf(file, ", i32* %%%s", tmpVarName);

      fprintf(file, "\n\tstore i32 %%%s, i32* @%s", tmpVarName, currentVarName);

      tmpVarCounter++;
      sprintf(tmpVarName, "tmp%d", tmpVarCounter);
    }
    else if (node->right->left->left->type == VAL)
    {
      /* Don't touch the following 3 lines :) */
      fseek(file, 0, SEEK_CUR);
      generateLLVMCode(node->left);
      fseek(file, -2, SEEK_CUR);
      sprintf(tmpVarName, "tmp%d", tmpVarCounter);

      fprintf(file, "\t");
      fprintf(file, "%%%s = alloca i32\n", tmpVarName);
      fprintf(file, "\tstore i32 ");
      generateLLVMCode(node->right->left->left);
      fprintf(file, ", i32* %%%s", tmpVarName);

      fprintf(file, "\n\tstore i32 %%%s, i32* @%s", tmpVarName, currentVarName);

      tmpVarCounter++;
      sprintf(tmpVarName, "tmp%d", tmpVarCounter);
    }
    // Times and Division
    else if (node->right->left->type == TIMES || node->right->left->type == DIV)
    {
      fprintf(file, "\t");
      generateLLVMCode(node->left);
      fprintf(file, " = ");
      generateLLVMCode(node->right->left);
      fprintf(file, "\n\tstore i32 %%%s, i32* @%s", currentVarName, currentVarName);
    }

    addVariable(currentVarName); // Change me later
    break;

  /* Conditional */
  case CONTROL:
    generateLLVMCode(node->left);
    generateLLVMCode(node->right);
    break;
  case WHILE:
    fprintf(file, "br label %%while%d.cond\n\n", whileCounter);

    // While condition
    fprintf(file, "while%d.cond:\n", whileCounter);
    fprintf(file, "\t%%cmp%d = ", cmpCounter);
    generateLLVMCode(node->left);
    fprintf(file, "\tbr i1 %%cmp%d, label %%while%d.body, label %%while%d.end\n", cmpCounter, whileCounter, whileCounter);

    // While body
    fprintf(file, "while%d.body:\n", whileCounter);
    generateLLVMCode(node->right);
    fprintf(file, "\tbr label %%while%d.cond\n", whileCounter);

    // While end
    fprintf(file, "while%d.end:\n", whileCounter);

    whileCounter++;
    cmpCounter++;
    break;
  case IF:
    fprintf(file, "\t%%cmp%d = ", cmpCounter);
    generateLLVMCode(node->left);
    fprintf(file, "\tbr i1 %%cmp%d, label %%if%d.then, label %%if%d.end\n\n", cmpCounter, ifCounter, ifCounter);

    fprintf(file, "if%d.then:\n", ifCounter);
    generateLLVMCode(node->right);
    fprintf(file, "\tbr label %%if%d.end\n", ifCounter);
    fprintf(file, "if%d.end:\n", ifCounter);

    cmpCounter++;
    ifCounter++;
    break;

  /* Logical operators */
  case LOGOR:
    fprintf(file, "or i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    fprintf(file, "\n");
    break;
  case LOGAND:
    fprintf(file, "and i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    fprintf(file, "\n");
    break;

  /* Compare operators */
  case COPLE:
    fprintf(file, "icmp sle i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    fprintf(file, "\n");
    break;
  case COPGE:
    fprintf(file, "icmp sge i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    fprintf(file, "\n");
    break;
  case COPL:
    fprintf(file, "icmp slt i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    fprintf(file, "\n");
    break;
  case COPG:
    fprintf(file, "icmp sgt i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    fprintf(file, "\n");
    break;
  case COPEQ:
    fprintf(file, "icmp eq i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    fprintf(file, "\n");
    break;
  case COPNEQ:
    fprintf(file, "icmp ne i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    fprintf(file, "\n");
    break;

  /* Arithmetic */
  case PLUS:
    fprintf(file, "add i32 ");
    generateLLVMCode(node->left->left->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right->left);
    break;
  case MINUS:
    fprintf(file, "sub i32 ");
    generateLLVMCode(node->left->left->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right->left);
    break;
  case TIMES:
    fprintf(file, "mul i32 ");
    generateLLVMCode(node->left->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    break;
  case DIV:
    fprintf(file, "sdiv i32 "); // using sdiv signed division, alternative is udiv for unsigned
    generateLLVMCode(node->left->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    break;

  /* Leafs */
  case VAL:
    fprintf(file, "%d", ((struct astLeafInt *)node)->value);
    break;
  case ID:
    fprintf(file, "%%%s", ((struct astLeafStr *)node)->string);
    currentVarName = ((struct astLeafStr *)node)->string;
    break;
  case EMPTY:
    break;
  default:
    printf("Error: unsupported type: %d\n", node->type);
    break;
  }
}

void generateLLVMFile(struct ast *node)
{
  generateLLVMCode(node);
  /* Change when symbol table */
  for (int i = 0; i < 25; i++)
  {
    if (strcmp(variables[i], "") != 0)
      fprintf(file, "@%s = global i32 0\n", variables[i]);
  }
  fprintf(file, "\ndefine i32 @main() {\n");
  fprintf(file, "entry:\n");
  fprintf(file, "\tcall void @setup()\n");
  fprintf(file, "\tcall void @mainloop()\n");
  fprintf(file, "\tret i32 0\n");
  fprintf(file, "}\n\n");
}

#endif