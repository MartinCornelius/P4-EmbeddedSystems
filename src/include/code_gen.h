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
    case PRINT:
        fprintf(file, "\t%%__tmpGlobal_");
        generateCode(node->left);
        fprintf(file, " = load i32, i32* @");
        generateCode(node->left);
        fprintf(file, "\n\tcall i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %%__tmpGlobal_");
        generateCode(node->left);
        fprintf(file, ");\n");
        tmpVarCounter++;
        break;

    /* Control structures */
    case WHILE:
        break;

    case DECL:
        if (node->right == NULL)
          return;
        else
          generateCode(node->right);
        break;

    case ASSIGN:
        // Check if constant
        if (node->right->type == VAL)
        {
          fprintf(file, "\t%%__const%d = alloca i32\n", tmpVarCounter);
          fprintf(file, "\tstore i32 ");
          generateCode(node->right);
          fprintf(file, ", i32* %%__const%d\n", tmpVarCounter);
          fprintf(file, "\t%%__tmp%d = load i32, i32* %%__const%d\n", tmpVarCounter, tmpVarCounter);
          tmpVarCounter++;
        }
        else
        {
          generateCode(node->right);
        }
        fprintf(file, "\n\tstore i32 %%__tmp%d, i32* @", tmpVarCounter-1);
        generateCode(node->left);
        tmpVarCounter++;
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
                fprintf(file, "%%__tmp%d, ", tmpVarCounter - 1);
                generateCode(node->right);
                fprintf(file, "\n");
            }
            else
            {
                generateCode(node->right);
                fprintf(file, "\t%%__tmp%d = sub i32 %%__tmp%d, %%__tmp%d\n", tmpVarCounter, tmp, tmpVarCounter-1);
            }
        }
        else
        {
            fprintf(file, "\t%%__tmp%d = sub i32 ", tmpVarCounter);
            generateCode(node->left);
            fprintf(file, ", ");
            generateCode(node->right);
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
  fprintf(file, "@pfmt = constant [4 x i8] c\"%%d\\0A\\00\"\n");
  fprintf(file, "declare i32 @printf(i8*,...)\n\n");

  // Global variables
  for (int i = 0; i < hTable->size; i++)
  {
    if (hTable->items[i] != NULL)
      fprintf(file, "@%s = global i32 0\n", hTable->items[i]->key);
  }
  fprintf(file, "\n");

  generateCode(node);
  fprintf(file, "\ndefine i32 @main() {\n");
  fprintf(file, "entry:\n");
  fprintf(file, "\tcall void @setup()\n");
  fprintf(file, "\tcall void @mainloop()\n");
  fprintf(file, "\tret i32 0\n");
  fprintf(file, "}\n\n");
}

#endif
