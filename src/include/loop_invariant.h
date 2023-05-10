#ifndef LOOP_INVARIANT_H
#define LOOP_INVARIANT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ast.h"
#include "symbol_table.h"
#include "type2text.h"

struct HashTables* symTable;
int optimizationDone = 5;
int loopInvariantVariableName = 0;
int dublecallpreventer = 0;
int scope = -1;

void loopInvariant(struct ast *node);
struct ast* loopInvariantFinder(struct ast *node);
bool varListContainsName(char *name);
bool containsVarByName(struct ast *node);
void printAssignedVarNameList();
void findAllAssignedVariables(struct ast *node);
struct ast* replaceWithLoopInvariantVariable();
struct HashTables*  createLoopInvariantSymbol(char* name, enum types type, int scope);

struct assignedVarName
{
    char* name;
    struct assignedVarName *next;
};

struct assignedVarName *assignedVarHandle;

void printAssignedVarNameList(){
    struct assignedVarName *tmp = malloc(sizeof(struct assignedVarName));
    tmp = assignedVarHandle;
    while(tmp != NULL){
        printf("Assigned var name: %s\n", tmp->name);
        tmp = tmp->next;
    }
}

void vacuumCleaner(struct ast *node) {
    if (node == NULL || node->type == ID || node->type == VAL || node->type == VALF || node->type == EMPTY)
        return;

    // Clean double lines
    if (node->type == LINES && node->left->type == LINES && (node->right == NULL || node->right->type == EMPTY))
    {
        node->right = node->left->right;
        node->left = node->left->left;
    }

    // If both children are null
    if (node->type == WHILE && node->right->type == LINES && node->right->left->type == EMPTY && node->right->right->type == EMPTY)
    {
        node->right = allocAST(EMPTY, NULL, NULL);
    }

    vacuumCleaner(node->left);
    vacuumCleaner(node->right);
}

void optimization(struct ast *root){
    symTable = fetchSymbolTable();
    printf("---- Running loop invariant optimization ----\n");
    while(optimizationDone > 0 ){
        scope = -1;
        loopInvariant(root);
        printAssignedVarNameList();
        printf("Cleaning list\n");
        assignedVarHandle->next = NULL;
        printAssignedVarNameList();
        optimizationDone--;
    }
    scope = -1;
    loopInvariant(root);
    assignedVarHandle->next = NULL;

    printf("---- Cleaning after loop invariant optimization ----\n");
    vacuumCleaner(root);
    vacuumCleaner(root);
}

void loopInvariant(struct ast *node)
{
    if (node == NULL || node->type == VAL || node->type == VALF || node->type == ID || node->type == EMPTY){
        return; 
    }

    //Missing func scope implementation
    if (node->type == SETUP || node->type == MAIN){
        scope++;
    }
    
    // The loop invariant assignment node
    if (node->left->type == WHILE){
        struct ast *assignmentNode = malloc(sizeof(struct ast));
        findAllAssignedVariables(node->left->right);
        assignmentNode = loopInvariantFinder(node->left->right);
        assignedVarHandle->next = NULL;

        if(assignmentNode != NULL){
            // Lav lines : left->assigmentNode : right->lines(while)    
            struct ast *whileLinesNode = allocAST(LINES, node->left, NULL);
            struct ast *rootLinesNode = allocAST(LINES, assignmentNode, whileLinesNode); 

            //printAssignedVarNameList();
            
            node->left = rootLinesNode;
            return;
        }
    }

    loopInvariant(node->left);
    loopInvariant(node->right);
}

void findAllAssignedVariables(struct ast *node){
    if (node == NULL || node->type == EMPTY || node->left == NULL){
            return;
    }
    if (node->left->type == ASSIGN){
        printf("Found assign\n");
        if(assignedVarHandle == NULL){
            assignedVarHandle = malloc(sizeof(struct assignedVarName));
            assignedVarHandle->name = "handle";
            assignedVarHandle->next = NULL;
        }
        //save node->left->left
        struct assignedVarName *tmp = malloc(sizeof(struct assignedVarName));
        if(assignedVarHandle->next == NULL){
            tmp->name = assignedVarHandle->name;
            tmp->next = NULL;
            assignedVarHandle->next = tmp;
        }
        else {tmp = assignedVarHandle;}

        while(tmp->next != NULL)
        {
            tmp = tmp->next;
        }

        struct assignedVarName *newAssignment = malloc(sizeof(struct assignedVarName));
        newAssignment->name = ((struct astLeafStr *)node->left->left)->string;
        newAssignment->next = NULL;

        tmp->next = newAssignment;

        printf("Variable name: %s\n", newAssignment->name);
    }
    if(node->left->type == LINES || node->left->type == WHILE){
        findAllAssignedVariables(node->left);
    }
    findAllAssignedVariables(node->right);
}
// Input while->right lines => traverse => if assignment is loop invariant return it.
struct ast* loopInvariantFinder(struct ast *node)
{
    if (node == NULL || node->type == EMPTY || node->left->type == WHILE || node->left->type == IF ||
        node->left == NULL){
            return NULL;
    }

    if (node->left->type == ASSIGN && node->left->right->type != ID && node->left->right->type != VAL && node->left->right->type != VALF) {
        //Check if node->left->right contains any saved node
        printf("Priting list\n");
        printAssignedVarNameList();
        bool result = containsVarByName(node->left->right);
        printf("The result: %d\n", result);
        if (!result)
        {
            struct ast *astNode = allocAST(ASSIGN, replaceWithLoopInvariantVariable(node->left), node->left->right);
            node->left->right = replaceWithLoopInvariantVariable(node->left);
            printf("Replace complete\n");
            printf("%s\n",((struct astLeafStr *)astNode->right)->string);
            loopInvariantVariableName++;
            return astNode; 
        }
    }
    if(node->left->type == LINES)
        return loopInvariantFinder(node->left);

    return loopInvariantFinder(node->right);
}

// Returns true if variable name is in node
// e.g. used to find if left side var is in right side expr.
bool containsVarByName(struct ast *node)
{
    if (node == NULL || node->type == VAL || node->type == VALF || node->type == EMPTY){
        return false;
    }
    char *nodeName = ((struct astLeafStr *)node)->string;
    if (node->type == ID && varListContainsName(nodeName) != 1)
        return false;
    if (varListContainsName(nodeName) == 1)
        return true;
    return containsVarByName(node->left) || containsVarByName(node->right);
}

bool varListContainsName(char *name)
{
    struct assignedVarName *tmp = malloc(sizeof(struct assignedVarName));
    tmp = assignedVarHandle;
    while (tmp != NULL)
    {
        if (strcmp(tmp->name, name) == 0)
            return true;
        tmp = tmp->next;
    }
    return false;
}

struct ast* replaceWithLoopInvariantVariable(struct ast *node){
    //recieves an assignment node
    //replaces rightside with a temp var that contains the right side
    char namedecl[30];
    sprintf(namedecl, "loopInvariantVariable%d", loopInvariantVariableName);
    char* name = namedecl;
    printf("Created loop invariant variable: %s\n", name);
    printf("Current scope: %d", scope);

    //Searces the symbol table for all symbols of same name
    if(dublecallpreventer %2 == 0){
        int currentType;
        
        if(symTable->hTable[scope] != NULL){
            printf("Searching for variable:%s in scope:%d\n", ((struct astLeafStr *)node->left)->string, scope);

            currentType = searchSymbol(symTable->hTable[scope], ((struct astLeafStr *)node->left)->string).type;
        }
        
        
        symTable = createLoopInvariantSymbol(name, currentType, scope);

        printTables(symTable);
            
    }
    
    dublecallpreventer++;
    return allocASTLeafStr(ID, name);
}

struct HashTables* createLoopInvariantSymbol(char* name, enum types type, int scope)
{
    struct HashTable* table = symTable->hTable[scope];

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
        printf("%s: Symbol created in scope %i\n\n", name, scope);

    return symTable;
}



#endif
