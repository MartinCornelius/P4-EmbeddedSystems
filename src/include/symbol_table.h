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

HashTables* createMainTable(int size)
{
    HashTables* symTable = (HashTables*)malloc(sizeof(HashTables));
    symTable->hTable = (HashTable*)malloc(sizeof(HashTable* ));
    return symTable;
}

HashTable* createTable(HashTables* symTable, int scope, int size)
{
    // hTables
    symTable->hTable = (HashTable*)malloc(sizeof(HashTable));
    symTable->hTable->items = (item**)malloc(sizeof(item* ));
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
    for (int i = 0; i < table->size; i++)
    {
        item* current = table->items[i];

        if (current != NULL)
        {
            printf("___________________\n");
            printf("Index: %d\n", i);
            printf("Name: %s, Type: %d\n", current->name, current->type);
            printf("___________________\n");
        }
    }
}

// Hashes each character in the key
int hashing(HashTable* table, char* name, int i)
{
    int hash = 537;
    while (name[i] != '\0')
    {
        hash = (hash + name[i] + i) % table->size;
        if (DEBUG)
            printf("        Hash: %d, Key: %c, i: %d\n", hash, name[i], i);
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

void insertSymbol(HashTable* table, char* name, enum types value)
{
    item* hItem = createItem(name, value);
    int index = hash(table, name);
    int i = 0;

    item* current = table->items[index];

    // Double hashing if collision
    while (current != NULL)
    {
        if (DEBUG)
            printf("%s: Insert collision at index %d, trying i%d, double hashing\n", name, index, i+1);
        
        index = doubleHash(table, name, ++i);
        current = table->items[index];
    }

    table->items[index] = hItem;
    table->count++;
}

enum types searchSymbol(HashTable* table, char* name)
{
    if (DEBUG) {
        printf("    %s: Searching in following table\n", name);

        // if (table)
        //     printTable(table);
    }


    int index = hash(table, name);
    item* current = table->items[index];
    int i = 0;

    if (current == NULL || current->name == NULL)
    {
        if (DEBUG)
            printf("    %s: Symbol not found\n", name);

        return not_found_enum;
    }

    // Double hashing until symbol is found or empty slot is found
    while (current != NULL && strcmp(current->name, name) != 0)
    {
        if (DEBUG)
            printf("    %s: Search collision at index %d, trying i%d, double hashing\n", name, index, i+1);
        
        index = doubleHash(table, name, ++i);
        current = table->items[index];
    }

    if (current == NULL)
    {
        if (DEBUG)
            printf("    %s: Symbol not found\n", name);

        return not_found_enum;
    }

    if (DEBUG)
        printf("    %s: Symbol Found! Type: %d\n", current->name, current->type);

    return current->type;
}

void createSymbol(HashTable* table, char* name, int type)
{
    if (DEBUG)
        printf("%s: Attempting to create symbol\n", name);

    if (table == NULL)
    {
        printf("Hash table missing \n");
        // TODO throw an error here
        return;
    }

    // Throw error if symbol already exists
    if (searchSymbol(table, name) != not_found_enum)
    {
        printf("ERROR: Declartion of two types of same name is not valid Name: %s\n\n", name);
        // TODO thorw an error here
        return;
    }

    if (DEBUG)
        printf("%s: Symbol created\n\n", name);

    insertSymbol(table, name, (enum types) type);
}

#endif