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
        case 6:
            strcpy(input, "float");
            break;
        default:
            printf("invalid type given, defaulting to flexint\n");
            strcpy(input, "flexint");
    }
}
