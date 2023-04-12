#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_types.h"
#include "type2text.h"

// TODO determine size at some point (Prime is ideal)
// TODO error handling
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
    HashTable* hTable = (HashTable*)malloc(sizeof(HashTable));
    hTable->size = size;
    hTable->count = 0;
    hTable->items = (item**)calloc(hTable->size, sizeof(item* ));
    return hTable;
}

item* createItem(char* key, enum types value)
{
    item* i = (item* )malloc(sizeof(item));
    i->key = key;
    i->value = value;
    return i;
}

// Hashes each character in the key
int hashing(HashTable* table, char* key, int i)
{
    int hash = 537;
    while (key[i] != '\0')
    {
        hash = (hash + key[i] + i) % table->size;
        printf("Hash: %d, Key: %c, i: %d\n", hash, key[i], i);
        i++;
    }

    return hash;
}

// These function make double hashing easier to implement
int hash(HashTable* table, char* key)
{
    return hashing(table, key, 0);
}

int doubleHash(HashTable* table, char* key, int i)
{
    return hashing(table, key, i);
}

void insertSymbol(HashTable* table, char* key, enum types value)
{
    item* hItem = createItem(key, value);
    int index = hash(table, key);
    int i = 0;

    item* current = table->items[index];

    // Double hashing if collision
    while (current != NULL)
    {
        printf("%s: Insert collision at index %d, trying i%d, double hashing\n", key, index, i+1);
        index = doubleHash(table, key, ++i);
        current = table->items[index];
    }

    table->items[index] = hItem;
    table->count++;
}

enum types searchSymbol(HashTable* table, char* key)
{
    printf("%s: Searching for symbol\n", key);
    int index = hash(table, key);
    item* current = table->items[index];
    int i = 0;

    if (current == NULL)
    {
        printf("%s: Symbol not found\n", key);
        return not_found_enum;
    }

    // Double hashing until symbol is found or empty slot is found
    while (current != NULL && strcmp(current->key, key) != 0)
    {
        printf("%s: Search collision at index %d, trying i%d, double hashing\n", key, index, i+1);
        index = doubleHash(table, key, ++i);
        current = table->items[index];
    }

    if (current == NULL)
    {
        printf("%s: Symbol not found\n", key);
        return not_found_enum;
    }

    printf("%s: Symbol Found! Type: %d\n", current->key, current->value);
    return current->value;
}

void createSymbol(HashTable* table, char* key, int value)
{
    if (table == NULL)
    {
        printf("Hash table missing \n");
        // TODO throw an error here
        return;
    }

    if (searchSymbol(table, key) == not_found_enum)
    {
        printf("%s: Symbol created\n", key);
        insertSymbol(table, key, (enum types) value);
    }
    else
    {
        printf("Declartion of two types of same name is not valid\n");
    }
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
            printf("Name: %s, Type: %d\n", current->key, current->value);
            printf("___________________\n");
        }
    }
}
