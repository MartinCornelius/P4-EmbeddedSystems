#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printToFile(char* path, char* programString)
{
    FILE *fp = fopen(path, "w");
    fprintf(fp, "%s", programString);
    fclose(fp);
}

void emit(const char* input, char* programString)
{
    strcat(programString, input);
}

void typeToString(char* input, int type)
{
    switch(type)
    {
        case 2:
            strcpy(input, "int8_t");
            break;
        case 3:
            strcpy(input, "int16_t");
            break;
        case 4:
            strcpy(input, "uint8_t");
            break;
        case 5:
            strcpy(input, "uint16_t");
            break;
        case 6:
            strcpy(input, "float");
            break;
        case 8:
            strcpy(input, "bool");
            break;
        case 9:
            strcpy(input, "char");
            break;
        case 10:
            strcpy(input, "flexint");
            break;
        default:
            printf("invalid type given, defaulting to flexint\n");
            strcpy(input, "flexint");
    }
}
