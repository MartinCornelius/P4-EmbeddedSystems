#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_types.h"


typedef struct item
{
    char* key;
    char* value;
} item;

typedef struct HashTable
{
    item** items;
    int size;
    int count;
} HashTable;

HashTable *hTable = NULL;

HashTable* createTable(int size)
{
    HashTable* hTable = malloc(sizeof(HashTable));
    hTable->size = size;
    hTable->count = 0;
    hTable->items = calloc(hTable->size, sizeof(item*));
    return hTable;
}

item* createItem(char* key, char* value)
{
    item* i = malloc(sizeof(item));
    i->key = key;
    i->value = value;
    return i;
}

int hash(HashTable* table, char* key)
{
    int hash = 0;
    int i = 0;
    while(key[i] != '\0')
    {
        hash = (hash + key[i] + i) % table->size;
        i++;
    }
    return hash;
}

void insert(HashTable* table, char* key, char* value)
{
    item* item = createItem(key, value);
    int index = hash(table, key);
    item* current = table->items[index];

    while (current != NULL)
    {
        index = hash(table, key);
        current = table->items[index];
    }


    table->items[index] = item;
    

    // if(current != NULL)
    // {
    //     while(current->next != NULL)
    //     {
    //         current = current->next;
    //     }
    //     current->next = i;
    // }
    // else
    // {
    //     table->items[index] = i;
    // }
    table->count++;
}

char* search(HashTable* table, char* key)
{
    int index = hash(table, key);
    item* current = table->items[index];
    while(current != NULL)
    {
        if(strcmp(current->key, key) == 0)
        {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}


Symbol_Struct* findSymbol(char* name)
{
    if(hTable == NULL) {
        // TODO error here
        printf("Hash table does not exist \n");
    }

    insert(hTable, "test", "test");

    // Symbol_Struct* temp = handle;
    // while(temp->next != NULL)
    // {
    //     temp = temp->next;
    //     if(strcmp(temp->name, name) == 0)
    //     {
    //         return temp;
    //     }
    // }
    return NULL;
}

void createSymbol(int type, char* name)
{
    // if variable hTable does not exists create it
    if(hTable == NULL) {
        HashTable *hTable = createTable(100);
        printf("Hash table created \n");
    }

    if(findSymbol(name) == NULL){

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
                // ((int8_Struct *)listHead)->next = newInt8Symbol;
                // listHead = newInt8Symbol;
                break;
            case float_enum:
                ; // is still necessary!!
                // float_Struct *newFloatSymbol = calloc(1, sizeof(float_Struct));
                // strcpy(newFloatSymbol->name, name);
                // newFloatSymbol->type = float_enum;
                // ((float_Struct *)listHead)->next = newFloatSymbol;
                // listHead = newFloatSymbol;
                break;
            default:
                printf("Illegal type (defaulting as flexint)\n");
                flexint_Struct *newFlexIntSymbol = calloc(1, sizeof(flexint_Struct));
                strcpy(newFlexIntSymbol->name, name);
                newFlexIntSymbol->type = flexint_enum;
                // ((flexint_Struct *)listHead)->next = newFlexIntSymbol;
                // listHead = newFlexIntSymbol;    
        }
        
        printf("Symbol created: %s\n", name);
    }
    else
    {
        printf("Declartion of two types of same name is not valid");
    }
}

void changeSymbolVal(char* name, char* sval, Symbol_Struct* handle)
{
    int val = atoi(sval);
    Symbol_Struct* symbol = findSymbol(name);
    if(symbol != NULL)
    {
        symbol->value = val;
    }
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
