#include "ast.h"

int scope;

typedef struct identifersStruct {
    char *name;
    struct identifersStruct *next;
} identifersStruct;

void typeCheck(char* name, struct ast* node, int level) {
    level++;
    printf("%s scope:%i level:%i\n", name, scope, level);

    // printAST(node->left->left, 5);

    struct ast *left = node->left;    
    struct ast *right = node->right;

    // if (left == NULL || right == NULL)
    //     level--;

    if (left == NULL && right == NULL) {
        level--;
        printf("Both left and right are NULL %i\n\n", level);

        return;
    }

    if (left) {
        printf("left\n");

        if (left->type == ID)
        {
            printf(" Found left: %s\n", ((struct astLeafStr *)left)->string);
        }

        typeCheck(name, left, level++);
    } else {
        level--;
    }

    if (right) {
        printf("right\n");

        if (right->type == ID)
        {
            printf(" Found right: %s\n", ((struct astLeafStr *)right)->string);
        }

        typeCheck(name, right, level++);
    } else {
        level--;
    }
}

void scopeChange(int newScope) {
    scope = newScope;
}