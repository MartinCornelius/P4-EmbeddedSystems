#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_types.h"

// TODO double hashing not working
// TODO determine size at some point
typedef struct item
{
    char* key;
    enum types value;
} item;

typedef struct HashTable
{
    item** items;
    int size;
    int count;
} HashTable;

HashTable* createTable(int size)
{
    HashTable* hTable = (HashTable*) malloc(sizeof(HashTable));
    hTable->size = size;
    hTable->count = 0;
    hTable->items = (item**) calloc(hTable->size, sizeof(item*));
    return hTable;
}

item* createItem(char* key, enum types value)
{
    item* i = (item*) malloc(sizeof(item));
    i->key = key;
    i->value = value;
    return i;
}

int hash(HashTable* table, char* key)
{
    int hash = 537;
    int i = 0;
    while(key[i] != '\0')
    {
        hash = (hash + key[i] + i) % table->size;
        i++;
    }


    return hash;
}

void insert(HashTable* table, char* key, enum types value)
{
    item* hItem = createItem(key, value);
    int index = hash(table, key);
    item* current = table->items[index];

    while (current != NULL)
    {
        // Double hashing
        index = hash(table, key);
        current = table->items[index];
    }


    table->items[index] = hItem;
    

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

enum types search(HashTable* table, char* key)
{
    int index = hash(table, key);
    item* current = table->items[index];
    while(current != NULL)
    {
        if(current->key == key)
        {
            return current->value;
        }

        // Double hashing if unable to find result
        index = hash(table, key);
        current = table->items[index];
    }
    return -1;
}


Symbol_Struct* findSymbol(HashTable* table, char* name)
{
    if(table == NULL) {
        // TODO error here
        printf("Hash table does not exist \n");
    }

    // insert(hTable, "name", "type");

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

void createSymbol(HashTable* table, int type, char* name)
{
    if(table == NULL) {
        printf("Hash table missing \n");
        // TODO throw an error here
        return;
    }

    if(findSymbol(table, name) == NULL){
        insert(table, name, (enum types) type);

        // switch(type)
        // {
        //     case -1:
        //         break;
        //     case input_enum:
        //         //Not implemented yet
        //         break;
        //     case output_enum:`
        //         //Not implemented
        //         break;
        //     case int8_enum:
        //         ; // is necessary!
        //         // int8_Struct *newInt8Symbol = calloc(1, sizeof(int8_Struct));
        //         // strcpy(newInt8Symbol->name, name);
        //         // newInt8Symbol->type = int8_enum;

        //         insert(table, name, int8_enum);
        //         // ((int8_Struct *)listHead)->next = newInt8Symbol;
        //         // listHead = newInt8Symbol;
        //         break;
        //     case float_enum:
        //         ; // is still necessary!!
        //         // float_Struct *newFloatSymbol = calloc(1, sizeof(float_Struct));
        //         // strcpy(newFloatSymbol->name, name);
        //         // newFloatSymbol->type = float_enum;
        //         // ((float_Struct *)listHead)->next = newFloatSymbol;
        //         // listHead = newFloatSymbol;
        //         break;
        //     default:
        //         printf("Illegal type (defaulting as flexint)\n");
        //         flexint_Struct *newFlexIntSymbol = calloc(1, sizeof(flexint_Struct));
        //         strcpy(newFlexIntSymbol->name, name);
        //         newFlexIntSymbol->type = flexint_enum;
        //         // ((flexint_Struct *)listHead)->next = newFlexIntSymbol;
        //         // listHead = newFlexIntSymbol;    
        // }
        
        printf("Symbol created: %s\n", name);
    }
    else
    {
        printf("Declartion of two types of same name is not valid");
    }
}

// void changeSymbolVal(char* name, char* sval, Symbol_Struct* handle)
// {
//     int val = atoi(sval);
//     Symbol_Struct* symbol = findSymbol(name);
//     if(symbol != NULL)
//     {
//         symbol->value = val;
//     }
// }

// void printTable(Symbol_Struct* handle)
// {
//     Symbol_Struct *temp = handle;
//     while(temp->next != NULL)
//     {
//         temp = temp->next;
//         printf("___________________\n");

//         printf("Name: %s, Type: %d, Value: %d\n", temp->name, temp->type, temp->value);

//         printf("___________________\n");
//     }
// }


void printTable(HashTable* table)
{
    for(int i = 0; i < table->size; i++)
    {
        item* current = table->items[i];
        if(current != NULL)
        {
            printf("___________________\n");
            printf("Index: %d\n", i);
            printf("Name: %s, Type: %d\n", current->key, current->value);
            printf("___________________\n");
        }
    }
}