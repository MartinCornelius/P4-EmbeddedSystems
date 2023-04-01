#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generateCode(struct ast *node)
{
    switch (node->type)
    {
    case ROOT:
        generateCode(node->left);
        generateCode(node->right);
        break;
    case SETUP:
        printf("setup {\n");
        generateCode(node->left);
        printf("}\n");
        break;
    case MAIN:
        printf("main {\n");
        generateCode(node->left);
        printf("}\n");
        break;
    case LINES:
        generateCode(node->left);
        printf(";\n");
        generateCode(node->right);
        break;
    case CONTROL:
        generateCode(node->left);
        printf("\n");
        generateCode(node->right);
        break;
    case IF:
        printf("if(");
        generateCode(node->left);
        printf(") {\n");
        generateCode(node->right);
        printf("}");
        break;
    case ELSEIF:
        printf("else if(");
        generateCode(node->left);
        printf("){\n");
        generateCode(node->right);
        printf("}");
        break;
    case ELSE:
        printf("else ");
        printf("{\n");
        generateCode(node->left);
        printf("}");
        break;
    case WHILE:
        printf("while(");
        generateCode(node->left);
        printf(") {\n");
        generateCode(node->right);
        printf("}");
        break;
    case LOGOR:
        generateCode(node->left);
        printf(" || ");
        generateCode(node->right);
        break;
    case LOGAND:
        generateCode(node->left);
        printf(" && ");
        generateCode(node->right);
        break;
    case COPGE:
        generateCode(node->left);
        printf(" >= ");
        generateCode(node->right);
        break;
    case COPLE:
        generateCode(node->left);
        printf(" <= ");
        generateCode(node->right);
        break;
    case COPEQ:
        generateCode(node->left);
        printf(" == ");
        generateCode(node->right);
        break;
    case COPNEQ:
        generateCode(node->left);
        printf(" != ");
        generateCode(node->right);
        break;
    case COPL:
        generateCode(node->left);
        printf(" < ");
        generateCode(node->right);
        break;
    case COPG:
        generateCode(node->left);
        printf(" > ");
        generateCode(node->right);
        break;
    case PLUS:
        printf("(");
        generateCode(node->left);
        printf(" + ");
        generateCode(node->right);
        printf(")");
        break;
    case MINUS:
        printf("(");
        generateCode(node->left);
        printf(" - ");
        generateCode(node->right);
        printf(")");
        break;
    case DIV:
        printf("(");
        generateCode(node->left);
        printf(" / ");
        generateCode(node->right);
        printf(")");
        break;
    case TIMES:
        printf("(");
        generateCode(node->left);
        printf(" * ");
        generateCode(node->right);
        printf(")");
        break;
    case VAL:
        printf("%d", ((struct astLeafInt *)node)->value);
        break;
    case VALF:
        printf("%f", ((struct astLeafFloat *)node)->value);
        break;
    case ID:
        printf("%s", ((struct astLeafStr *)node)->string);
        break;
    case ASSIGN:
        generateCode(node->left);
        printf(" = ");
        generateCode(node->right);
        break;
    case LARROW:
        generateCode(node->left);
        printf(" <- ");
        generateCode(node->right);
        break;
    case EMPTY:
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