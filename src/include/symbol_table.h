#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_types.h"

void createSymbol(int type, char* name, Symbol_Struct* handle, void* listHead)
{ 
    if(findSymbol(name, handle) == NULL){

        switch(type)
        {
            case -1:
                break;
            case input_enum:
                //Not implemented yet
                break;
            case output_enum:
                //Not implemented
                break;
            case int8_enum:
                ; // is necessary!
                int8_Struct *newInt8Symbol = calloc(1, sizeof(int8_Struct));
                strcpy(newInt8Symbol->name, name);
                newInt8Symbol->type = int8_enum;
                ((int8_Struct *)listHead)->next = newInt8Symbol;
                listHead = newInt8Symbol;
                break;
            case float_enum:
                ; // is still necessary!!
                float_Struct *newFloatSymbol = calloc(1, sizeof(float_Struct));
                strcpy(newFloatSymbol->name, name);
                newFloatSymbol->type = float_enum;
                ((float_Struct *)listHead)->next = newFloatSymbol;
                listHead = newFloatSymbol;
                break;
            default:
                printf("Illegal type (defaulting as flexint)\n");
                flexint_Struct *newFlexIntSymbol = calloc(1, sizeof(flexint_Struct));
                strcpy(newFlexIntSymbol->name, name);
                newFlexIntSymbol->type = flexint_enum;
                ((flexint_Struct *)listHead)->next = newFlexIntSymbol;
                listHead = newFlexIntSymbol;
        }
        
    }else{
        printf("Declartion of two types of same name is not valid");
    }
}

void changeSymbolVal(char* name, char* sval, Symbol_Struct* handle)
{
    int val = atoi(sval);
    Symbol_Struct* symbol = findSymbol(name, handle);
    if(symbol != NULL)
    {
        symbol->value = val;
    }
}

Symbol_Struct* findSymbol(char* name, Symbol_Struct* handle)
{
    Symbol_Struct* temp = handle;
    while(temp->next != NULL)
    {
        temp = temp->next;
        if(strcmp(temp->name, name) == 0)
        {
            return temp;
        }
    }
    return NULL;
}

void printTable(Symbol_Struct* handle)
{
    Symbol_Struct *temp = handle;
    while(temp->next != NULL)
    {
        temp = temp->next;
        printf("___________________\n");

        printf("Name: %s, Type: %d, Value: %d\n", temp->name, temp->type, temp->value);

        printf("___________________\n");
    }
}
