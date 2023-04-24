#include "ast.h"

typedef struct identifersStruct {
    char *name;
    struct identifersStruct *next;
} identifersStruct;


void typeCheck(char* name, struct ast* node, int level) {
    level++;
    if (DEBUG)
        printf("%s scope:%i level:%i\n", name, symTable->scope, level);

    // printAST(node->left->left, 5);

    struct ast *left = node->left;    
    struct ast *right = node->right;
    struct ast *dirNode[2] = {left, right};

    if (left == NULL || right == NULL) {
        if (DEBUG)
            printf("Both left and right are NULL %i\n", level);

        if (level == 0) {
            if (DEBUG)
                printf("DONE!!\n\n");

            HashTables* symTable = fetchSymbolTable();
        }

        return;
    }

    for (int i = 0; i < 2; i++)
    {
        if (dirNode[i]) {
            if (DEBUG)
                if (i == 0) {
                    printf("left\n");
                } else {
                    printf("right\n");
                }

            if (dirNode[i]->type == ID)
            {
                searchReturn searchAssign = searchSymbol(symTable->hTable[symTable->scope], name);
                searchReturn searchFound = searchSymbol(symTable->hTable[symTable->scope], ((struct astLeafStr *)dirNode[i])->string);
                
                if (DEBUG)
                    printf("Found: %s\n", ((struct astLeafStr *)dirNode[i])->string);

                if (searchAssign.type != searchFound.type) {
                    if (DEBUG)
                        printf("ERROR: Non matching types %s type != %s type!\n", name, ((struct astLeafStr *)dirNode[i])->string);
                        
                    char* errorStr;
                    sprintf(errorStr, "Non matching types %s != %s", getCustomType(searchAssign.type), getCustomType(searchFound.type));
                    yyerror(errorStr);
                    // yyerror the above correctly
                    
                }

                level--;
            }

            typeCheck(name, dirNode[i], level);
        }
    }
}