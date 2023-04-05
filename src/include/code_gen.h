#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ifCounter = 1;
int cmpCounter = 1;
int whileCounter = 1;
int tmpVarCounter = 1;
char *currentVarName;
char tmpVarName[30];
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

void generateCode(struct ast *node)
{
    if (node == NULL)
        return;    

    switch (node->type)
    {
    case ROOT:
        generateCode(node->left);
        generateCode(node->right);
        break;
    case SETUP:
        fprintf(file, "define void @setup() {\n");
        fprintf(file, "entry:\n");
        generateCode(node->left);
        fprintf(file, "\tret void\n");
        fprintf(file, "}\n\n");
        break;
    case MAIN:
        fprintf(file, "define void @mainloop() {\n");
        fprintf(file, "entry:\n");
        generateCode(node->left);
        fprintf(file, "\tret void\n");
        fprintf(file, "}\n\n");
        break;
    case LINES:
        generateCode(node->left);
        fprintf(file, "\n");
        generateCode(node->right);
        break;

    /* Control structures */
    case WHILE:
        break;

    case ASSIGN:
        generateCode(node->right);
        fprintf(file, "\n\tstore i32 %%__tmp%d, i32* @", tmpVarCounter-1);
        generateCode(node->left);
        tmpVarCounter++;
        addVariable(currentVarName); // Change me later
        break;

    /* Arithmetic */
    case PLUS:
        if (node->right->type != VAL)
        {
            generateCode(node->right);
            int tmp = tmpVarCounter - 1;
            if (node->left->type == VAL)
            {
                fprintf(file, "\t%%__tmp%d = add i32 %%__tmp%d, ", tmpVarCounter, tmpVarCounter - 1);
                generateCode(node->left);
                fprintf(file, "\n");
            }
            else
            {
                generateCode(node->left);
                fprintf(file, "\t%%__tmp%d = add i32 %%__tmp%d, %%__tmp%d\n", tmpVarCounter, tmpVarCounter-1, tmp);
            }
        }
        else if (node->left->type != VAL)
        {
            generateCode(node->left);
            int tmp = tmpVarCounter - 1;
            if (node->right->type == VAL)
            {
                fprintf(file, "\t%%__tmp%d = add i32 %%__tmp%d, ", tmpVarCounter, tmpVarCounter - 1);
                generateCode(node->right);
                fprintf(file, "\n");
            }
            else
            {
                generateCode(node->right);
                fprintf(file, "\t%%__tmp%d = add i32 %%__tmp%d, %%__tmp%d\n", tmpVarCounter, tmpVarCounter-1, tmp);
            }
        }
        else
        {
            fprintf(file, "\t%%__tmp%d = add i32 ", tmpVarCounter);
            generateCode(node->right);
            fprintf(file, ", ");
            generateCode(node->left);
            fprintf(file, "\n");
        }
        tmpVarCounter++;
        break;
    case MINUS:
        if (node->right->type != VAL)
        {
            generateCode(node->right);
            int tmp = tmpVarCounter - 1;
            if (node->left->type == VAL)
            {
                fprintf(file, "\t%%__tmp%d = sub i32 ", tmpVarCounter);
                generateCode(node->left);
                fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
            }
            else 
            {
                generateCode(node->left);
                fprintf(file, "\t%%__tmp%d = sub i32 %%__tmp%d, %%__tmp%d\n", tmpVarCounter, tmpVarCounter-1, tmp);
            }
        }
        else if (node->left->type != VAL)
        {
            generateCode(node->left);
            int tmp = tmpVarCounter - 1;
            if (node->right->type == VAL)
            {
                fprintf(file, "\t%%__tmp%d = sub i32 ", tmpVarCounter);
                generateCode(node->right);
                fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
            }
            else
            {
                generateCode(node->right);
                fprintf(file, "\t%%__tmp%d = sub i32 %%__tmp%d, %%__tmp%d\n", tmpVarCounter, tmpVarCounter-1, tmp);
            }
        }
        else
        {
            fprintf(file, "\t%%__tmp%d = sub i32 ", tmpVarCounter);
            generateCode(node->right);
            fprintf(file, ", ");
            generateCode(node->left);
            fprintf(file, "\n");
        }
        tmpVarCounter++;
        break;
    case TIMES:
        if (node->right->type != VAL)
        {
            generateCode(node->right);
            int tmp = tmpVarCounter - 1;
            if (node->left->type == VAL)
            {
                fprintf(file, "\t%%__tmp%d = mul i32 %%__tmp%d, ", tmpVarCounter, tmpVarCounter - 1);
                generateCode(node->left);
                fprintf(file, "\n");
            }
            else 
            {
                generateCode(node->left);
                fprintf(file, "\t%%__tmp%d = mul i32 %%__tmp%d, %%__tmp%d\n", tmpVarCounter, tmpVarCounter-1, tmp);
            }
        }
        else if (node->left->type != VAL)
        {
            generateCode(node->left);
            int tmp = tmpVarCounter - 1;
            if (node->right->type == VAL)
            {
                fprintf(file, "\t%%__tmp%d = mul i32 %%__tmp%d, ", tmpVarCounter, tmpVarCounter - 1);
                generateCode(node->right);
                fprintf(file, "\n");
            }
            else 
            {
                generateCode(node->right);
                fprintf(file, "\t%%__tmp%d = mul i32 %%__tmp%d, %%__tmp%d\n", tmpVarCounter, tmpVarCounter-1, tmp);
            }
        }
        else
        {
            fprintf(file, "\t%%__tmp%d = mul i32 ", tmpVarCounter);
            generateCode(node->right);
            fprintf(file, ", ");
            generateCode(node->left);
            fprintf(file, "\n");
        }
        tmpVarCounter++;
        break;
    case DIV:
        if (node->right->type != VAL)
        {
            generateCode(node->right);
            int tmp = tmpVarCounter - 1;
            if (node->left->type == VAL)
            {
                fprintf(file, "\t%%__tmp%d = sdiv i32 %%__tmp%d, ", tmpVarCounter, tmpVarCounter - 1);
                generateCode(node->left);
                fprintf(file, "\n");
            }
            else 
            {
                generateCode(node->left);
                fprintf(file, "\t%%__tmp%d = sdiv i32 %%__tmp%d, %%__tmp%d\n", tmpVarCounter, tmpVarCounter-1, tmp);
            }
        }
        else if (node->left->type != VAL)
        {
            generateCode(node->left);
            int tmp = tmpVarCounter - 1;
            if (node->right->type == VAL)
            {
                fprintf(file, "\t%%__tmp%d = sdiv i32 %%__tmp%d, ", tmpVarCounter, tmpVarCounter - 1);
                generateCode(node->right);
                fprintf(file, "\n");
            }
            else 
            {
                generateCode(node->right);
                fprintf(file, "\t%%__tmp%d = sdiv i32 %%__tmp%d, %%__tmp%d\n", tmpVarCounter, tmpVarCounter-1, tmp);
            }
        }
        else
        {
            fprintf(file, "\t%%__tmp%d = sdiv i32 ", tmpVarCounter);
            generateCode(node->left);
            fprintf(file, ", ");
            generateCode(node->right);
            fprintf(file, "\n");
        }
        tmpVarCounter++;
        break;

    case VAL:
        fprintf(file, "%d", ((struct astLeafInt *)node)->value);
        break;
    case ID:
        fprintf(file, "%s", ((struct astLeafStr *)node)->string);
        currentVarName = ((struct astLeafStr *)node)->string;
        break;
    case EMPTY:
        break;
    default:
        printf("Error: unsupported type\n");
        break;
    }
}

void generateFile(struct ast *node)
{
  generateCode(node);
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