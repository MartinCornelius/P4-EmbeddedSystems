#include "ast.h"

#define DEBUG_TYPE false

typedef struct identifersStruct {
    char *name;
    struct identifersStruct *next;
} identifersStruct;


void typeCheck(char* name, struct ast* node) {
    if (DEBUG_TYPE)
        printf("%s scope:%i\n", name, symTable->scope);

    struct ast *left = node->left;    
    struct ast *right = node->right;
    struct ast *dirNode[2] = {left, right};

    if (left == NULL || right == NULL) {
        if (DEBUG_TYPE)
            printf("Both left and right are NULL\n");

        return;
    }

    // Search both left and right
    for (int i = 0; i < 2; i++)
    {
        if (dirNode[i]) {
            if (DEBUG_TYPE)
                if (i == 0) {
                    printf("left\n");
                } else {
                    printf("right\n");
                }

            if (dirNode[i]->type == ID)
            {
                struct searchReturn searchAssign = searchSymbol(symTable->hTable[symTable->scope], name);
                struct searchReturn searchFound = searchSymbol(symTable->hTable[symTable->scope], ((struct astLeafStr *)dirNode[i])->string);

                // If variable on right side is not found in scope search in global scope
                if (searchFound.type == not_found_enum)
                    searchFound = searchSymbol(symTable->hTable[0], ((struct astLeafStr *)dirNode[i])->string);
                
                if (DEBUG_TYPE)
                    printf("Found: %s\n", ((struct astLeafStr *)dirNode[i])->string);

                if (searchAssign.type != searchFound.type) {
                    if (DEBUG_TYPE)
                        printf("ERROR: Non matching types %s type != %s type!\n", name, ((struct astLeafStr *)dirNode[i])->string);
                        
                    char errorStr[100]; 
                    sprintf(errorStr, "Non matching types %d != %d", searchAssign.type, searchFound.type);
                    yyerror(errorStr);
                }
            }

            typeCheck(name, dirNode[i]);
        }
    }
}