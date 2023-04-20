#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_types.h"
#include "type2text.h"

#define DEBUG false


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
} HashTable;

typedef struct HashTables
{
    HashTable** hTable;
    int scope;
} HashTables;

HashTables* symTable;

HashTables* createMainTable(int size)
{
    symTable = (HashTables*)malloc(sizeof(HashTables));
    symTable->hTable = (HashTable**)malloc(size * sizeof(HashTable*));
    symTable->scope = -1;
    return symTable;
}

HashTable* createTable(int size)
{
    if (DEBUG)
        printf("Creating a new table\n");

    HashTable* hTable = (HashTable*)malloc(sizeof(HashTable));
    hTable->items = (item**)malloc(size * sizeof(item* ));
    hTable->size = size;
    hTable->count = 0;
    return hTable;
}

item* createItem(char* name, enum types type)
{
    item* i = (item* )malloc(sizeof(item));
    i->name = name;
    i->type = type;
    return i;
}

void printTable(HashTable* table, int indent)
{
    printf("%*s--------------------------\n", indent, "");
    for (int i = 0; i < table->size; i++)
    {
        item* current = table->items[i];

        if (current != NULL)
        {
            printf("%*sIndex: %d Name: %s, Type: %d\n", indent, "", i, current->name, current->type);
        }
    }
    printf("%*s--------------------------\n", indent, "");
}

void printTables(HashTables* symTable)
{
    for (int i = 0; i < symTable->scope; i++)
    {
        printf("Scope: %d\n", i);
        printTable(symTable->hTable[i], 0);
    }
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
            printf("        Hash: %2d, Key: %c, plus: %d\n", hash, name[i], plus);

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
    if (DEBUG)
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

    printTable(table, 8);
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
            printTable(table, 8);
    }


    int index = hash(table, name);
    item* current = table->items[index];
    int i = 0;

    searchReturn sReturn = {not_found_enum, index};

    // check if current is null
    if (current == NULL)
    {
        if (DEBUG)
            printf("    %s: Symbol not found\n", name);

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

    return sReturn;
}

// void createSymbol(HashTable* table, char* name, enum types type)
HashTables* createSymbol(char* name, enum types type)
{
    HashTable* table = symTable->hTable[symTable->scope];

    if (DEBUG)
        printf("%s: Attempting to create symbol\n", name);

    if (table == NULL)
    {
        printf("Hash table missing \n");
        // TODO throw an error here
        return symTable;
    }

    searchReturn search = searchSymbol(table, name);


    if (DEBUG)
        printf("    %i: Search returned type: %d\n", search.hashIndex, search.type);

    // Throw error if symbol already exists
    if (search.type != not_found_enum)
    {
        printf("ERROR: Declartion of two types of same name is not valid Name: %s\n\n", name);
        // TODO thorw an error here
        return symTable;
    }

    insertSymbol(table, search.hashIndex, name, type);

    if (DEBUG)
        printf("%s: Symbol created in scope %i\n\n", name, symTable->scope);

    return symTable;
}


// TODO This creates a table too much
// TODO determine size of table at some time
HashTables* changeScope(char* call)
{
    if (DEBUG)
        printf("Exiting scope %s\n", call);

    symTable->scope++;
    symTable->hTable[symTable->scope] = createTable(100);

    return symTable;
}

HashTables* fetchSymbolTable() {
    return symTable;
}

#endif