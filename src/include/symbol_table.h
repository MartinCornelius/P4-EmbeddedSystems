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
struct item
{
    char* name;
    enum types type;
};

struct HashTable
{
    struct item** items;
    int size;
    int count;
};

struct HashTables
{
    struct HashTable** hTable;
    int scope;
};


struct HashTables *symTable;

void printTable(struct HashTable* table, int indent);

struct HashTables* createMainTable(int size)
{
    symTable = calloc(1, sizeof(struct HashTables));
    symTable->hTable = calloc(size, sizeof(struct HashTable*));
    symTable->scope = -1;
    return symTable;
}

struct HashTable* createTable(int size)
{
    if (DEBUG)
        printf("Creating a new table\n");

    struct HashTable* hTable = calloc(size, sizeof(struct HashTable*));
    hTable->items = calloc(size, sizeof(struct item*));
    hTable->size = size;
    hTable->count = 0;


    return hTable;
}

struct item* createItem(char* name, enum types type)
{
    struct item* i = calloc(1 ,sizeof(struct item));
    i->name = name;
    i->type = type;
    return i;
}

void printTable(struct HashTable* table, int indent)
{
    printf("%*s--------------------------\n", indent, "");
    // printf("The Table size is %d\n", table->size);
    for (int i = 0; i < table->size; i++)
    {
        struct item* current = table->items[i];

        if (current != NULL && current->name != NULL)
        {
            printf("%*sIndex: %d ", indent, "", i);
            printf("Name: %s ", current->name);
            printf("Type: %d\n",  current->type);
            
        }
    }
    printf("%*s--------------------------\n", indent, "");
}

void printTables(struct HashTables* symTable)
{
    for (int i = 0; i < symTable->scope; i++)
    {
        printf("Scope: %d\n", i);
        printTable(symTable->hTable[i], 0);
    }
}

// Hashes each character in the key
int hashing(struct HashTable* table, char* name, int plus)
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
int hash(struct HashTable* table, char* name)
{
    return hashing(table, name, 0);
}

int doubleHash(struct HashTable* table, char* name, int i)
{
    if (DEBUG)
        printf("    Attempting Double Hashing\n");

    return hashing(table, name, i);
}

// This should only be called after a search (passing the result from search to index in this funciton)
// Since no checks are implementet in this function
void insertSymbol(struct HashTable* table, int index, char* name, enum types value)
{
    struct item* current = table->items[index];
    struct item* hItem = createItem(name, value);
    int i = 0;

    table->items[index] = hItem;
    table->count++;
}

// Used to return both type and the hash index
struct searchReturn
{
    enum types type;
    int hashIndex;
};


struct searchReturn searchSymbol(struct HashTable* table, char* name)
{
    if (DEBUG) {
        printf("    %s: Searching in following table\n", name);

        if (table)
            printTable(table, 8);
    }


    int index = hash(table, name);
    struct item* current = table->items[index];
    int i = 0;

    struct searchReturn sReturn = {not_found_enum, index};

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
struct HashTables* createSymbol(char* name, enum types type)
{
    struct HashTable* table = symTable->hTable[symTable->scope];

    if (DEBUG)
        printf("%s: Attempting to create symbol\n", name);

    if (table == NULL)
    {
        printf("Hash table missing \n");
        // TODO throw an error here
        return symTable;
    }

    struct searchReturn search = searchSymbol(table, name);


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
struct HashTables* changeScope(char* call)
{
    if (DEBUG)
        printf("Exiting scope %s\n", call);
    

    symTable->scope++;
    
    symTable->hTable[symTable->scope] = createTable(5000);

    return symTable;
}

struct HashTables* fetchSymbolTable() {
    return symTable;
}

#endif
