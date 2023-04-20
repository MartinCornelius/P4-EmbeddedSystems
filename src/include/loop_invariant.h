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
bool isLoopVariant(struct ast *node);
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
        printf("assignedvar: %s\n", tmp->name);
        tmp = tmp->next;
    }
}

void optimization(struct ast *root){
    while(optimizationDone > 0 ){
        loopInvariant(root);
        printf("WE ARE GOING AGAIN\n");
        printAssignedVarNameList();
        assignedVarHandle->next = NULL;
        printAssignedVarNameList();
        optimizationDone--;
    }
    loopInvariant(root);
    assignedVarHandle->next = NULL;
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
            printf("assignmentNode result: %s\n", printType(assignmentNode->type));
            // Lav lines : left->assigmentNode : right->lines(while)    
            struct ast *whileLinesNode = allocAST(LINES, node->left, NULL);
            struct ast *rootLinesNode = allocAST(LINES, assignmentNode, whileLinesNode); 

            printAssignedVarNameList();
            
            node->left = rootLinesNode;

            printf("%d\n", optimizationDone);
        }
    }
    
    printf("going left\n");
    loopInvariant(node->left);
    printf("going right\n");
    loopInvariant(node->right);
}

// Input while->right lines => traverse => if assignment is loop invariant return it.
struct ast* loopInvariantFinder(struct ast *node)
{
    printf("loopInvariantFinder is called\n");
    printf("loopInv type : %s\n", printType(node->type));
    if (node == NULL || node->type == EMPTY || node->left->type == WHILE || node->left->type == IF ||
        node->left == NULL){
            printf("returning null : %s\n", printType(node->type));
            return NULL;
        }
        
    printf("We go here\n");
    if (node->left->type == ASSIGN) {
        if(assignedVarHandle == NULL){
            assignedVarHandle = malloc(sizeof(struct assignedVarName));
            assignedVarHandle->name = "handle";
            assignedVarHandle->next = NULL;
        }
        printf("Assign found\n");
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
        printf("contains result: %d\n", result);
        if (!result)
        {
            struct ast *astNode = node->left; 
            node->left = allocAST(EMPTY, NULL, NULL);
            printf("removed node before returning\n");
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

bool isLoopVariant(struct ast *node)
{
    return true;
}

#endif