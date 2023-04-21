#ifndef LOOP_INVARIANT_H
#define LOOP_INVARIANT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ast.h"
#include "type2text.h"

int optimizationDone = 5;

void loopInvariant(struct ast *node);
struct ast* loopInvariantFinder(struct ast *node);
bool varListContainsName(char *name);
bool containsVarByName(struct ast *node);
void printAssignedVarNameList();

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
        tmp = tmp->next;
    }
}

void vacuumCleaner(struct ast *node) {
    if (node == NULL || node->type == ID || node->type == VAL || node->type == VALF || node->type == EMPTY)
        return;

    // Clean double lines
    if (node->type == LINES && node->left->type == LINES)
    {
        node->right = node->left->right;
        node->left = node->left->left;
    }

    // Clean empty && lines
    if (node->type == LINES && node->left->type == EMPTY)
    {
        node->left = node->right;
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
    printf("---- Running loop invariant optimization ----\n");
    while(optimizationDone > 0 ){
        loopInvariant(root);
        printAssignedVarNameList();
        assignedVarHandle->next = NULL;
        printAssignedVarNameList();
        optimizationDone--;
    }
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
    
    // The loop invariant assignment node
    struct ast *assignmentNode = malloc(sizeof(struct ast));
    if (node->left->type == WHILE){
        assignmentNode = loopInvariantFinder(node->left->right);

        if(assignmentNode != NULL){
            // Lav lines : left->assigmentNode : right->lines(while)    
            struct ast *whileLinesNode = allocAST(LINES, node->left, NULL);
            struct ast *rootLinesNode = allocAST(LINES, assignmentNode, whileLinesNode); 

            printAssignedVarNameList();
            
            node->left = rootLinesNode;
        }
    }

    loopInvariant(node->left);
    loopInvariant(node->right);
}

// Input while->right lines => traverse => if assignment is loop invariant return it.
struct ast* loopInvariantFinder(struct ast *node)
{
    if (node == NULL || node->type == EMPTY || node->left->type == WHILE || node->left->type == IF ||
        node->left == NULL){
            return NULL;
    }

    if (node->left->type == ASSIGN) {
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

        //Check if node->left->right contains any saved node
        bool result = containsVarByName(node->left->right);
        printAssignedVarNameList();
        if (!result)
        {
            struct ast *astNode = node->left; 
            node->left = allocAST(EMPTY, NULL, NULL);
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

#endif