#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generateCode(struct ast *node)
{
    switch (node->type)
    {
    case PLUS:
        printf("(");
        generateCode(node->left);
        printf("+");
        generateCode(node->right);
        printf(")");
        break;
    case VAL:
        printf("%d", ((struct astLeaf *)node)->value);
        break;
    case LINES:
        generateCode(node->left);
        printf(";");
        generateCode(node->right);
        break;
    case ID:
        printf("%s", ((struct astLeafStr *)node)->string);
        break;
    case ASSIGN:
        generateCode(node->left);
        printf("=");
        generateCode(node->right);
        break;
    default:
        printf("Error: unsupported type\n");
        break;
    }
}

void printToFile(char *path, char *programString)
{
    FILE *fp = fopen(path, "w");
    fprintf(fp, "%s", programString);
    fclose(fp);
}

void emit(const char *input, char *programString)
{
    strcat(programString, input);
}

void typeToString(char *input, int type)
{
    switch (type)
    {
    case input_enum:
        strcpy(input, "input");
        break;
    case output_enum:
        strcpy(input, "output");
        break;
    case int8_enum:
        strcpy(input, "int8_t");
        break;
    case int16_enum:
        strcpy(input, "int16_t");
        break;
    case int32_enum:
        strcpy(input, "int");
        break;
    case uint8_enum:
        strcpy(input, "uint8_t");
        break;
    case uint16_enum:
        strcpy(input, "uint16_t");
        break;
    case uint32_enum:
        strcpy(input, "unsigned int");
        break;
    case float_enum:
        strcpy(input, "float");
        break;
    case bool_enum:
        strcpy(input, "bool");
        break;
    case char_enum:
        strcpy(input, "char");
        break;
    case flexint_enum:
        strcpy(input, "flexint");
        break;
    default:
        printf("invalid type given, defaulting to flexint\n");
        strcpy(input, "flexint");
    }
}

#endif