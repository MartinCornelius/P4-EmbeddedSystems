#include "ast.h"

#define DEBUG_TYPE false

void typeCheck(char*, struct ast*);
void checkNode(char*, struct ast*);

void typeCheck(char* name, struct ast* node) {
    if (DEBUG_TYPE)
        printf("%s scope:%i\n", name, symTable->scope);

    if (!node || !node->left || !node->right) {
        if (DEBUG_TYPE)
            printf("Both left and right are NULL\n");

        return;
    }

    if (node->type != ID) {
        checkNode(name, node->left);
        checkNode(name, node->right);
    }

}

void checkNode(char* name, struct ast* node) {
    if (node->type == ID)
    {
        printf("%s\n", ((struct astLeafStr *)node)->string);
        struct searchReturn searchAssign = searchSymbol(symTable->hTable[symTable->scope], ((struct astLeafStr *)node)->string);

        // If variable without type declartion check global scope for variable
        if (searchAssign.type == not_found_enum){
            searchAssign = searchSymbol((symTable->hTable[0]), ((struct astLeafStr *)node)->string);
        }


        if (searchAssign.type == not_found_enum)
        {
            printf("Error: Variable not declared %s\n", ((struct astLeafStr *)node)->string);
            exit(1);
        }

        struct searchReturn searchFound = searchSymbol(symTable->hTable[symTable->scope], ((struct astLeafStr *)node)->string);

        // If variable on right side is not found in scope search in global scope
        if (searchFound.type == not_found_enum)
            searchFound = searchSymbol(symTable->hTable[0], ((struct astLeafStr *)node)->string);
        
        if (DEBUG_TYPE)
            printf("Found: %s\n", ((struct astLeafStr *)node)->string);

        if (searchAssign.type != searchFound.type) {
            if (DEBUG_TYPE)
                printf("ERROR: Non matching types %s type != %s type!\n", name, ((struct astLeafStr *)node)->string);
                
            char errorStr[100]; 
            sprintf(errorStr, "Non matching types %d != %d", searchAssign.type, searchFound.type);
            yyerror(errorStr);
        }

        typeCheck(name, node);
    }

}