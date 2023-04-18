#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_types.h"
#include "type2text.h"

#define DEBUG true

// TODO determine size at some point (Prime is ideal)
// TODO error handling
typedef struct item
{
    char* name;
    enum types type;
} item;

typedef struct HashTable
{
    item** items;
    int size;
    int count;
    int scope;
} HashTable;

typedef struct HashTables
{
    HashTable* hTable;
} HashTables;

// TODO size might be different fix tihs also in function below
HashTables* createMainTable(int size)
{
    HashTables* symTable = (HashTables*)malloc(sizeof(HashTables));
    symTable->hTable = (HashTable*)malloc(size * sizeof(HashTable* ));
    return symTable;
}

HashTable* createTable(HashTables* symTable, int scope, int size)
{
    // hTables
    symTable->hTable = (HashTable*)malloc(sizeof(HashTable));
    symTable->hTable->items = (item**)malloc(size * sizeof(item* ));
    symTable->hTable->size = size;
    symTable->hTable->count = 0;
    symTable->hTable->scope = scope;
    return symTable->hTable;
}

item* createItem(char* name, enum types type)
{
    item* i = (item* )malloc(sizeof(item));
    i->name = name;
    i->type = type;
    return i;
}

void printTable(HashTable* table)
{
    printf("___________________\n");
    for (int i = 0; i < table->size; i++)
    {
        item* current = table->items[i];

        if (current != NULL)
        {
            printf("Index: %d Name: %s, Type: %d\n", i, current->name, current->type);
        }
    }
    printf("___________________\n");
}

// Hashes each character in the key
int hashing(HashTable* table, char* name, int plus)
{
    int hash = 537;
    int i = 0;

    while (name[i] != '\0')
    {
        hash = (hash + name[i] + plus) % table->size;
        if (DEBUG)
            printf("        Hash: %d, Key: %c, plus: %d\n", hash, name[i], plus);

        i++;
    }

    return hash;
}

// These function make double hashing easier to implement
int hash(HashTable* table, char* name)
{
    return hashing(table, name, 0);
}

int doubleHash(HashTable* table, char* name, int i)
{
    printf("    Attempting Double Hashing\n");

    return hashing(table, name, i);
}

// This should only be called after a search (passing the result from search to index in this funciton)
// Since no checks are implementet in this function
void insertSymbol(HashTable* table, int index, char* name, enum types value)
{
    item* current = table->items[index];
    item* hItem = createItem(name, value);
    int i = 0;

    table->items[index] = hItem;
    table->count++;
}

// Used to return both type and the hash index
typedef struct searchReturn
{
    enum types type;
    int hashIndex;
} searchReturn;


searchReturn searchSymbol(HashTable* table, char* name)
{
    if (DEBUG) {
        printf("    %s: Searching in following table\n", name);

        if (table)
            printTable(table);
    }


    int index = hash(table, name);
    item* current = table->items[index];
    int i = 0;

    searchReturn sReturn = {not_found_enum, index};

    // check if current is null
    if (current == NULL)
    {
        if (DEBUG)
            printf("    %s: ay Symbol not found\n", name);

        return sReturn;
    }

    // Double hashing until symbol is found or empty slot is found
    while (current != NULL)
    {
        if (current->name != NULL || current->name == name) {
            break;
        }

        if (DEBUG)
            printf("    %s: Search collision at index %d, trying i%d, double hashing\n", name, index, i+1);
        
        index = doubleHash(table, name, ++i);
        current = table->items[index];
    }

    if (DEBUG)
        printf("    %s: Symbol Found! Type: %d\n", current->name, current->type);

    sReturn.type = current->type;
    sReturn.hashIndex = index;

    printf("index: %d\n", index);
    
    return sReturn;
}

void createSymbol(HashTable* table, char* name, enum types type)
{
    if (DEBUG)
        printf("%s: Attempting to create symbol\n", name);

    if (table == NULL)
    {
        printf("Hash table missing \n");
        // TODO throw an error here
        return;
    }

    searchReturn search = searchSymbol(table, name);


    if (DEBUG)
        printf("    %i: Search returned type: %d\n", search.hashIndex, search.type);

    // Throw error if symbol already exists
    if (search.type != not_found_enum)
    {
        printf("ERROR: Declartion of two types of same name is not valid Name: %s\n\n", name);
        // TODO thorw an error here
        return;
    }

    insertSymbol(table, search.hashIndex, name, type);

    if (DEBUG)
        printf("%s: Symbol created\n\n", name);
}

#endif