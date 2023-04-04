#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ifCounter = 1;
int cmpCounter = 1;
int whileCounter = 1;
char *currentVarName;

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
    case ASSIGN:
        fprintf(file, "\t");
        generateCode(node->left);
        fprintf(file, " = ");
        generateCode(node->right);
        fprintf(file, "\n\tstore i32 %%%s, i32* @%s", currentVarName, currentVarName);
        break;

    /* Control structures */
    case WHILE:
        fprintf(file, "br label %%while%d.cond\n\n", whileCounter);
        fprintf(file, "\t%%cmp%d = ", cmpCounter);
        generateCode(node->left);


        generateCode(node->right);
        break;

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
        printf("Error: unsupported type\n");
        break;
    }
}

void generateFile(struct ast *node)
{
  generateCode(node);
  fprintf(file, "\ndefine i32 @main() {\n");
  fprintf(file, "entry:\n");
  fprintf(file, "\tcall void @setup()\n");
  fprintf(file, "\tcall void @mainloop()\n");
  fprintf(file, "\tret i32 0\n");
  fprintf(file, "}\n\n");
}

#endif