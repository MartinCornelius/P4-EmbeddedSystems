#ifndef LOOP_INVARIANT_H
#define LOOP_INVARIANT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ast.h"
#include "type2text.h"

int optimizationDone = 1;
int loopInvariantVariableName = 0;

void loopInvariant(struct ast *node);
struct ast* loopInvariantFinder(struct ast *node);
bool varListContainsName(char *name);
bool containsVarByName(struct ast *node);
void printAssignedVarNameList();
void findAllAssignedVariables(struct ast *node);
struct ast* replaceWithLoopInvariantVariable();

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
    printf("---- Running loop invariant optimization ----\n");
    while(optimizationDone > 0 ){
        loopInvariant(root);
        printAssignedVarNameList();
        printf("Cleaning list\n");
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
        printf("WHYYYYY!!!\n");
        printf("Calling find all variables\n");
        findAllAssignedVariables(node->left->right);
        printf("Wooop!\n");
        assignmentNode = loopInvariantFinder(node->left->right);

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
    printf("Finding assigned variables\n");
    if (node == NULL || node->type == EMPTY ||
        node->left == NULL){
            printf("Returning\n");
            return;
    }
    printf("Standing on node type: %d\n", node->type);
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
    if(node->left->type == LINES){
        printf("Moving left\n");
        findAllAssignedVariables(node->left);
    }

    printf("Moving right\n");
    findAllAssignedVariables(node->right);
    printf("Branch done\n");
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
            //struct ast *astNode = node->left; 
            struct ast *astNode = allocAST(ASSIGN, replaceWithLoopInvariantVariable(), node->left->right);
            printf("The node returning is: %d\n", node->left->type);
            node->left->right = replaceWithLoopInvariantVariable();
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


struct ast* replaceWithLoopInvariantVariable(){
    //recieves an assignment
    //replaces rightside with a temp var that contains the right side
    char namedecl[30];
    sprintf(namedecl, "loopInvariantVariable%d", loopInvariantVariableName);
    printf("%s\n", namedecl);
    char* name = namedecl;
    printf("%s\n", name);
    return allocASTLeafStr(ID, name);

}

#endif
