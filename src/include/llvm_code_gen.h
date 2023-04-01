#ifndef LLVM_CODE_GEN
#define LLVM_CODE_GEN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generateLLVMCode(struct ast *node)
{
  if (node == NULL)
    return;

  switch (node->type)
  {
  case ROOT:
    generateLLVMCode(node->left);
    generateLLVMCode(node->right);
    break;

  case SETUP:
    fprintf(file, "define void @setup() {\n");
    fprintf(file, "entry:\n");
    generateLLVMCode(node->left);
    fprintf(file, "\tret void\n");
    fprintf(file, "}\n\n");
    break;
  case MAIN:
    fprintf(file, "define void @mainloop() {\n");
    fprintf(file, "entry:\n");
    generateLLVMCode(node->left);
    fprintf(file, "\tret void\n");
    fprintf(file, "}\n\n");
    break;
  case LINES:
    generateLLVMCode(node->left);
    fprintf(file, "\n");
    generateLLVMCode(node->right);
    break;
  case ASSIGN:
    fprintf(file, "\t");
    generateLLVMCode(node->left);
    fprintf(file, " = ");
    generateLLVMCode(node->right);
    break;
  case PLUS:
    fprintf(file, "add i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    break;
  case MINUS:
    fprintf(file, "sub i32 ");
    generateLLVMCode(node->left);
    fprintf(file, ", ");
    generateLLVMCode(node->right);
    break;
  case VAL:
    fprintf(file, "%d", ((struct astLeafInt *)node)->value);
    break;
  case ID:
    fprintf(file, "%%%s", ((struct astLeafStr *)node)->string);
    break;
  case EMPTY:
    break;
  default:
    printf("Error: unsupported type\n");
    break;
  }
}

void generateLLVMFile(struct ast *node)
{
  generateLLVMCode(node);
  fprintf(file, "define i32 @main() {\n");
  fprintf(file, "entry:\n");
  fprintf(file, "\tcall void @setup()\n");
  fprintf(file, "\tcall void @mainloop()\n");
  fprintf(file, "\tret i32 0\n");
  fprintf(file, "}\n");
}

#endif