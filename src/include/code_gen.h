#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"

int casted = 0;
int ifCounter = 1;
int startingIfCounter = 1;
int cmpCounter = 1;
int whileCounter = 1;
int tmpVarCounter = 1;
int globalVarCounter = 1;
int currentScope = -1;
char *currentVarName;
char *currentType;
char tmpVarName[30];
int itemType = 0;

struct HashTables *symTable;

void loadParams(struct ast *node);
void loadLocalParams(struct ast *node);

int globalCheck(char *string);

void arthemticOperation(struct ast *node, char *operation);
void compareFunc( struct ast *node, char *operation);

void generateCode(struct ast *node)
{
	symTable = fetchSymbolTable();

	if (node == NULL)
		return;

	switch (node->type)
	{
	case ROOT:
		generateCode(((struct astRootNode *)node)->setup);
		generateCode(((struct astRootNode *)node)->mainloop);
		generateCode(((struct astRootNode *)node)->funcs);
		break;
	case SETUP:
		currentScope++;
		// fprintf(file, "define void @setup() {\n");
		// fprintf(file, "entry:\n");
		generateCode(node->left);
		// fprintf(file, "\tret void\n");
		// fprintf(file, "}\n\n");
		break;
	case MAIN:
		currentScope++;
		fprintf(file, "define void @mainloop() {\n");
		fprintf(file, "entry:\n");
		generateCode(node->left);
		fprintf(file, "\tret void\n");
		fprintf(file, "}\n\n");
		break;
	case FUNCS:
		currentScope++;
		generateCode(node->left);
		fprintf(file, "\n");
		generateCode(node->right);
		break;
	case FUNC:
		fprintf(file, "define void @");
		generateCode(((struct astFuncNode *)node)->id);
		fprintf(file, "(");
		generateCode(((struct astFuncNode *)node)->parameters);
		fprintf(file, ") {\nentry:\n");

		// Allocate parameters on stack
		loadLocalParams(((struct astFuncNode *)node)->parameters);

		generateCode(((struct astFuncNode *)node)->body);

		fprintf(file, "\tret void\n}\n");
		break;
	case FUNCCALL:
		// Load scope variables
		fprintf(file, "\n\t; Load scope variables before func call\n");
		loadParams(node->right);

		fprintf(file, "\tcall void @");
		generateCode(node->left);
		fprintf(file, "(");
		generateCode(node->right);
		fprintf(file, ")\n");
		break;
	case PARAMS:
		if (node->left->type == VAL)
		{
			fprintf(file, "i32 ");
			generateCode(node->left);
		}
		else if (node->left->type == ID)
		{
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type, 1);
			fprintf(file, "%s %%", currentType);
			generateCode(node->left);
		}
		else
		{
			generateCode(node->left);
		}

		if (node->right != NULL)
		{
			fprintf(file, ", ");
			if (node->right->type != PARAMS)
			{
				currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->right)->string).type, 2);
				fprintf(file, "%s %%", currentType);
			}
			generateCode(node->right);
		}
		break;
	case LINES:
		if (node->left->type == IF)
			startingIfCounter = ifCounter;
		generateCode(node->left);
		fprintf(file, "\n");
		generateCode(node->right);
		break;
	case PRINT:
		casted = 0;
		// currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type);

		struct searchReturn search = searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string);

		// If variable without type declartion check global scope for variable
		if (search.type == not_found_enum)
		{
			search = searchSymbol((symTable->hTable[0]), ((struct astLeafStr *)node->left)->string);
		}

		if (search.type == not_found_enum)
		{
			printf("Error: Variable %s not declared\n", ((struct astLeafStr *)node->left)->string);
			exit(1);
		}

		currentType = typeConverter(search.type, 3);

		fprintf(file, "\t%%__tmpGlobal_%d", globalVarCounter);
		generateCode(node->left);
		fprintf(file, " = load %s, %s* %%sc%d_", currentType, currentType, currentScope);
		generateCode(node->left);

		if (strcmp(currentType, "i32") != 0)
		{
			// Check if signed
			char *checksigned = getCustomType(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type);
			// Type convertion
			if (checksigned[0] == 'u')
				fprintf(file, "\n\t%%__castGlobal_%d%s = zext %s %%__tmpGlobal_%d%s to i32", globalVarCounter, currentVarName, currentType, globalVarCounter, currentVarName);
			else
				fprintf(file, "\n\t%%__castGlobal_%d%s = sext %s %%__tmpGlobal_%d%s to i32", globalVarCounter, currentVarName, currentType, globalVarCounter, currentVarName);
			casted = 1;
		}

		fprintf(file, "\n\tcall i32(i8*,...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @pfmt, i32 0, i32 0), i32 %s%d", (casted == 0 ? "%__tmpGlobal_" : "%__castGlobal_"), globalVarCounter);
		generateCode(node->left);
		fprintf(file, ");\n");
		globalVarCounter++;
		tmpVarCounter++;
		break;

	/* Control structures */
	case WHILE:
		// Get current type
		if (node->left->left->type == ID)
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left->left)->string).type, 4);
		else if (node->left->right->type == ID)
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left->right)->string).type, 5);
		else
			currentType = "i32";

		int tmpWhileCounter = whileCounter;
		int tmpCmpCounter = cmpCounter;

		fprintf(file, "br label %%while%d.cond\n\n", tmpWhileCounter);

		// While condition
		fprintf(file, "while%d.cond:\n", tmpWhileCounter);
		generateCode(node->left);
		fprintf(file, "\tbr i1 %%cmp%d, label %%while%d.body, label %%while%d.end\n", tmpCmpCounter, tmpWhileCounter, tmpWhileCounter);

		whileCounter++;
		cmpCounter++;

		// While body
		fprintf(file, "while%d.body:\n", tmpWhileCounter);
		generateCode(node->right);
		fprintf(file, "\tbr label %%while%d.cond\n", tmpWhileCounter);

		// While end
		fprintf(file, "while%d.end:\n", tmpWhileCounter);

		break;

	case DECL:
		itemType = searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type;
		currentType = typeConverter(itemType, 6);

		// Global Scope
		if (currentScope == 0) {
			char floatOrInt[10];

			if ((typeConverter(itemType, 8) == "half") || (typeConverter(itemType, 9) == "float") || (typeConverter(itemType, 10) == "double")) {
				sprintf(floatOrInt, "%d", ((struct astLeafFloat *)node->right)->value);
			} else {
				sprintf(floatOrInt, "%d", ((struct astLeafInt *)node->right)->value);
			}

			fprintf(file, "@%s = global %s %s\n", ((struct astLeafStr *)node->left)->string, typeConverter(itemType, 11), floatOrInt);

			break;
		}

		// Check if constant
		if (node->right->type == VAL)
	{
			fprintf(file, "\t%%__const%d = alloca %s\n", tmpVarCounter, currentType);
			fprintf(file, "\tstore %s ", currentType);
			generateCode(node->right);
			fprintf(file, ", %s* %%__const%d\n", currentType, tmpVarCounter);
			fprintf(file, "\t%%__tmp%d = load %s, %s* %%__const%d\n", tmpVarCounter, currentType, currentType, tmpVarCounter);
			tmpVarCounter++;
		}
		else if (node->right->type == VALF)
		{
			fprintf(file, "\t%%__const%d = alloca %s\n", tmpVarCounter, currentType);
			fprintf(file, "\tstore %s ", currentType);
			generateCode(node->right);
			fprintf(file, ", %s* %%__const%d\n", currentType, tmpVarCounter);
			fprintf(file, "\t%%__tmp%d = load %s, %s* %%__const%d\n", tmpVarCounter, currentType, currentType, tmpVarCounter);
			tmpVarCounter++;
		}
		else if (node->right->type == ID)
		{
			fprintf(file, "\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->right);
			tmpVarCounter++;
		}
		else
		{
			generateCode(node->right);
		}

		// Allocate the variable for the current scope
		fprintf(file, "\n\t; Allocate scope variable\n");
		fprintf(file, "\t%%sc%d_", currentScope);
		generateCode(node->left);
		fprintf(file, " = alloca %s\n", currentType);

		fprintf(file, "\n\tstore %s %%__tmp%d, %s* %%sc%d_", currentType, tmpVarCounter - 1, currentType, currentScope);
		generateCode(node->left);
		tmpVarCounter++;
		break;

	case IF:
		// Single if statement
		if (((struct astIfNode *)node)->right == NULL)
		{
			int tmpIfCounter = ifCounter++;
			// Comparison
			generateCode(((struct astIfNode *)node)->left);
			fprintf(file, "\tbr i1 %%cmp%d, label %%if%d.then, label %%if%d.end\n\n", cmpCounter, tmpIfCounter, startingIfCounter);
			cmpCounter++;

			// If body
			fprintf(file, "if%d.then:\n", tmpIfCounter);
			generateCode(((struct astIfNode *)node)->middle);
			fprintf(file, "\tbr label %%if%d.end\n", tmpIfCounter);

			// If end
			fprintf(file, "if%d.end:\n", tmpIfCounter);
		}
		// Else
		else
		{
			int tmpIfCounter = ifCounter++;
			// Comparison
			generateCode(((struct astIfNode *)node)->left);
			fprintf(file, "\tbr i1 %%cmp%d, label %%if%d.then, label %%if%d.else\n\n", cmpCounter, tmpIfCounter, tmpIfCounter);
			cmpCounter++;

			// If body
			fprintf(file, "if%d.then:\n", tmpIfCounter);
			generateCode(((struct astIfNode *)node)->middle);
			fprintf(file, "\tbr label %%if%d.end\n", tmpIfCounter);

			// Else
			fprintf(file, "if%d.else:\n", tmpIfCounter);
			generateCode(((struct astIfNode *)node)->right);
			fprintf(file, "\tbr label %%if%d.end\n", tmpIfCounter);

			// If end
			fprintf(file, "if%d.end:\n", tmpIfCounter);
		}
		break;

	case ASSIGN:
	{
		int isGlobalLeft = globalCheck(((struct astLeafStr *)node->left)->string);
		struct searchReturn search = searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string);

		// If variable without type declartion check global scope for variable
		if (search.type == not_found_enum)
		{
			search = searchSymbol((symTable->hTable[0]), ((struct astLeafStr *)node->left)->string);
		}

		if (search.type == not_found_enum)
		{
			printf("Error: Variable %s not declared\n", ((struct astLeafStr *)node->left)->string);
			exit(1);
		}

		currentType = typeConverter(search.type, 7);

		// Check if constant
		if (node->right->type == VAL)
		{
			fprintf(file, "\t%%__const%d = alloca %s\n", tmpVarCounter, currentType);
			fprintf(file, "\tstore %s ", currentType);
			generateCode(node->right);
			fprintf(file, ", %s* %%__const%d\n", currentType, tmpVarCounter);
			fprintf(file, "\t%%__tmp%d = load %s, %s* %%__const%d\n", tmpVarCounter, currentType, currentType, tmpVarCounter);
			tmpVarCounter++;
		}
		else if (node->right->type == VALF)
		{
			fprintf(file, "\t%%__const%d = alloca %s\n", tmpVarCounter, currentType);
			fprintf(file, "\tstore %s ", currentType);
			generateCode(node->right);
			fprintf(file, ", %s* %%__const%d\n", currentType, tmpVarCounter);
			fprintf(file, "\t%%__tmp%d = load %s, %s* %%__const%d\n", tmpVarCounter, currentType, currentType, tmpVarCounter);
			tmpVarCounter++;
		}
		else if (node->right->type == ID)
		{
			fprintf(file, "\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->right);
			tmpVarCounter++;
		}
		else
		{
			generateCode(node->right);
		}
		// Load variable
		if (isGlobalLeft) {
			fprintf(file, "\n\tstore %s %%__tmp%d, %s* @", currentType, tmpVarCounter - 1, currentType, currentScope);
		} else {
			fprintf(file, "\n\tstore %s %%__tmp%d, %s* %%sc%d_", currentType, tmpVarCounter - 1, currentType, currentScope);
		}

		generateCode(node->left);
		tmpVarCounter++;
		break;
	}
	/* Arithmetic */
	case PLUS:
		arthemticOperation(node, "add");
		break;
	case MINUS:
		arthemticOperation(node, "sub");
		break;
	case TIMES:
		arthemticOperation(node, "mul");
		break;
	case DIV:
		arthemticOperation(node, "div");
		break;

	/* Compare operators */
	case COPLE:
		compareFunc(node, "sle");
		break;
	case COPGE:
		compareFunc(node, "sge");
		break;
	case COPL:
		compareFunc(node, "slt");
		break;
	case COPG:
		compareFunc(node, "sgt");
		break;
	case COPEQ:
		compareFunc(node, "eq");
		break;
	case COPNEQ:
		compareFunc(node, "ne");
		break;

	case VAL:
		fprintf(file, "%d", ((struct astLeafInt *)node)->value);
		break;
	case VALF:
		fprintf(file, "%.22g", ((struct astLeafFloat *)node)->value);
		break;
	case ID:
		fprintf(file, "%s", ((struct astLeafStr *)node)->string);
		currentVarName = ((struct astLeafStr *)node)->string;
		break;
	case EMPTY:
		break;
	default:
		printf("Error: unsupported type\n");
		break;
	}
}

void generateFile(struct ast *node)
{
	fprintf(file, "@pfmt = constant [4 x i8] c\"%%d\\0A\\00\"\n");
	fprintf(file, "declare i32 @printf(i8*,...)\n\n");

	generateCode(node);
	fprintf(file, "\ndefine i32 @main() {\n");
	fprintf(file, "entry:\n");
	// fprintf(file, "\tcall void @setup()\n");
	fprintf(file, "\tcall void @mainloop()\n");
	fprintf(file, "\tret i32 0\n");
	fprintf(file, "}\n\n");
}

void loadParams(struct ast *node)
{
	if (node == NULL || node->left->type == VAL)
		return;

	else if (node->left->type == ID)
	{
		printf("type %s\n", currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type, 12));
		currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type, 13);
		fprintf(file, "\t%%");
		generateCode(node->left);
		fprintf(file, " = load %s, %s* %%sc%d_", currentType, currentType, currentScope);
		generateCode(node->left);
		fprintf(file, "\n");
	}
	else
	{
		loadParams(node->left);
	}

	if (node->right != NULL)
	{
		if (node->right->type != PARAMS)
		{
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->right)->string).type, 14);
			fprintf(file, "\t%%", currentScope);
			generateCode(node->right);
			fprintf(file, " = load %s, %s* %%sc%d_", currentType, currentType);
			generateCode(node->right);
			fprintf(file, "\n");
		}
		loadParams(node->right);
	}
}

void loadLocalParams(struct ast *node)
{
	if (node == NULL || node->left->type == VAL)
		return;

	else if (node->left->type == ID)
	{
		currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type, 15);
		fprintf(file, "\t; Load parameters into stack variables\n");
		fprintf(file, "\t%%sc%d_", currentScope);
		generateCode(node->left);
		fprintf(file, " = alloca %s\n", currentType);

		// Store value in stack
		fprintf(file, "\tstore %s %%", currentType);
		generateCode(node->left);
		fprintf(file, ", %s* %%sc%d_", currentType, currentScope);
		generateCode(node->left);
		fprintf(file, "\n\n");
	}
	else
	{
		loadLocalParams(node->left);
	}

	if (node->right != NULL)
	{
		if (node->right->type != PARAMS)
		{
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->right)->string).type, 16);
			fprintf(file, "\t; Load parameters into stack variables\n");
			fprintf(file, "\t%%sc%d_", currentScope);
			generateCode(node->right);
			fprintf(file, " = alloca %s\n", currentType);

			// Store value in stack
			fprintf(file, "\tstore %s %%", currentType);
			generateCode(node->right);
			fprintf(file, ", %s* %%sc%d_", currentType, currentScope);
			generateCode(node->right);
			fprintf(file, "\n\n");
		}
		loadLocalParams(node->right);
	}
}

// COPLE COPGE COPL COPG COPEQ COPNEQ
void compareFunc(struct ast *node, char *operation) {
	// node->type

	if (node->left->type == ID)
	{
		struct searchReturn globalCheck = searchSymbol(symTable->hTable[0], ((struct astLeafStr *)node->left)->string);
		int isGlobal = 0;

		// If variable without type declartion check global scope for variable
		if (globalCheck.type != not_found_enum)
		{
			isGlobal = 1;
		}

		// Load variable
		if (isGlobal) {
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
		} else {
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
		}
		generateCode(node->left);
		if (node->right->type == ID)
		{
			tmpVarCounter++;
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->right);
			fprintf(file, "\n\t%%cmp%d = icmp %s %s %%__tmp%d, %%__tmp%d", cmpCounter, operation, currentType, tmpVarCounter - 1, tmpVarCounter);
		}
		else
		{
			fprintf(file, "\n\t%%cmp%d = icmp %s %s %%__tmp%d, ", cmpCounter, operation, currentType, tmpVarCounter);
			generateCode(node->right);
		}
		tmpVarCounter++;
	}
	else if (node->right->type == ID)
	{
		struct searchReturn globalCheck = searchSymbol(symTable->hTable[0], ((struct astLeafStr *)node->right)->string);
		int isGlobal = 0;

		// If variable without type declartion check global scope for variable
		if (globalCheck.type != not_found_enum)
		{
			isGlobal = 1;
		}

		// Load variable
		if (isGlobal) {
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
		} else {
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
		}
		
		generateCode(node->right);
		fprintf(file, "\n\t%%cmp%d = icmp %s %s ", cmpCounter, operation, currentType);
		generateCode(node->left);
		fprintf(file, ", %%__tmp%d", tmpVarCounter);
		tmpVarCounter++;
	}
	else
	{
		fprintf(file, "\n\t%%cmp%d = icmp %s %s ", cmpCounter, operation, currentType);
		generateCode(node->left);
		fprintf(file, ", ");
		generateCode(node->right);
	}
	fprintf(file, "\n");
}


int globalCheck(char *string) {
	struct searchReturn searchCheck = searchSymbol(symTable->hTable[0], string);

	// If variable without type declartion check global scope for variable
	if (searchCheck.type != not_found_enum)
	{
		return 1;
	}

	return 0;
}


void arthemticOperation(struct ast *node, char *operation) {
	// All other opertaions is just op or f + op but div is sdiv and fdiv
	char *operation2 = operation;

	if (operation == "div") {
		operation2 = "sdiv";
	}


	if (node->right->type == ID)
	{
		if (node->left->type == ID || node->left->type == VAL || node->left->type == VALF) {
			if (node->left->type == ID) {
				int isGlobalLeft = globalCheck(((struct astLeafStr *)node->left)->string);
				int isGlobalRight = globalCheck(((struct astLeafStr *)node->right)->string);

				// Load variable
				if (isGlobalLeft) {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType, currentScope);
				} else {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				}

				generateCode(node->left);
				tmpVarCounter++;
				// Load variable
				if (isGlobalLeft) {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType, currentScope);
				} else {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				}

				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = f%s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation, currentType, tmpVarCounter - 1, tmpVarCounter - 2);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = %s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation2, currentType, tmpVarCounter - 1, tmpVarCounter - 2);
				}
			} else if (node->left->type == VAL || node->left->type == VALF) {
				int isGlobalRight = globalCheck(((struct astLeafStr *)node->right)->string);

				// Load variable
				if (isGlobalRight) {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType, currentScope);
				} else {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				}

				generateCode(node->right);
				tmpVarCounter++;

				if (node->left->type == VAL)
				{
					// Make addition operation
					fprintf(file, "\n\t%%__tmp%d = %s %s ", tmpVarCounter, operation2, currentType);
					generateCode(node->left);
					fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
				}
				else if (node->left->type == VALF)
				{
					// Make addition operation
					fprintf(file, "\n\t%%__tmp%d = f%s %s ", tmpVarCounter, operation, currentType);
					generateCode(node->left);
					fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
				}
			}
		}
		else
		{
			int isGlobalRight = globalCheck(((struct astLeafStr *)node->right)->string);

			generateCode(node->left);
			// Load variable
			if (isGlobalRight) {
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType, currentScope);
			} else {
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			}
			generateCode(node->right);
			tmpVarCounter++;
			if (currentType[0] == 'f')
			{
				fprintf(file, "\n\t%%__tmp%d = f%s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation, currentType, tmpVarCounter - 1, tmpVarCounter - 2);
			}
			else
			{
				fprintf(file, "\n\t%%__tmp%d = %s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation2, currentType, tmpVarCounter - 1, tmpVarCounter - 2);
			}
		}
	}
	else if (node->left->type == ID)
	{
		if (node->right->type == ID || node->right->type == VAL || node->right->type == VALF) {
			if (node->right->type == ID) {
				int isGlobalLeft = globalCheck(((struct astLeafStr *)node->left)->string);
				int isGlobalRight = globalCheck(((struct astLeafStr *)node->right)->string);

				if (isGlobalRight) {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType, currentScope);
				} else {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				}

				generateCode(node->right);
				tmpVarCounter++;

				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				
				// Make addition operation
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = f%s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = %s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation2, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			} else if (node->right->type == VAL || node->right->type == VALF) {
				int isGlobalLeft = globalCheck(((struct astLeafStr *)node->left)->string);

				if (isGlobalLeft) {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType, currentScope);
				} else {
					fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				}

				generateCode(node->left);
				tmpVarCounter++;

				if (node->right->type == VAL)
				{
					// Make addition operation
					fprintf(file, "\n\t%%__tmp%d = %s %s %%__tmp%d, ", tmpVarCounter, operation2, currentType, tmpVarCounter - 1);
					generateCode(node->right);
					fprintf(file, "\n");
				}
				else if (node->right->type == VALF)
				{
					// Make addition operation
					fprintf(file, "\n\t%%__tmp%d = f%s %s %%__tmp%d, ", tmpVarCounter, operation, currentType, tmpVarCounter - 1);
					generateCode(node->right);
					fprintf(file, "\n");
				}
			}
		}
		else
		{
			generateCode(node->right);
			// Load variable
			int isGlobalLeft = globalCheck(((struct astLeafStr *)node->left)->string);
			if (isGlobalLeft) {
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType, currentScope);
			} else {
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			}
			
			generateCode(node->left);
			tmpVarCounter++;
			if (currentType[0] == 'f')
			{
				fprintf(file, "\n\t%%__tmp%d = f%s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
			}
			else
			{
				fprintf(file, "\n\t%%__tmp%d = %s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation2, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
			}
		}
	}
	else if (node->right->type != VAL)
	{
		generateCode(node->right);
		int tmp = tmpVarCounter - 1;
		if (node->left->type == VAL)
		{
			fprintf(file, "\t%%__tmp%d = %s %s %%__tmp%d, ", tmpVarCounter, operation2, currentType, tmpVarCounter - 1);
			generateCode(node->left);
			fprintf(file, "\n");
		}
		else
		{
			generateCode(node->left);
			fprintf(file, "\t%%__tmp%d = %s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation2, currentType, tmpVarCounter - 1, tmp);
		}
	}
	else if (node->left->type != VAL)
	{
		generateCode(node->left);
		int tmp = tmpVarCounter - 1;
		if (node->right->type == VAL)
		{
			fprintf(file, "\t%%__tmp%d = %s %s %%__tmp%d, ", tmpVarCounter, operation2, currentType, tmpVarCounter - 1);
			generateCode(node->right);
			fprintf(file, "\n");
		}
		else
		{
			generateCode(node->right);
			fprintf(file, "\t%%__tmp%d = %s %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, operation2, currentType, tmpVarCounter - 1, tmp);
		}
	}
	else
	{
		if (currentType[0] == 'f')
		{
			fprintf(file, "\t%%__tmp%d = f%s %s ", tmpVarCounter, operation, currentType);
		}
		else
		{
			fprintf(file, "\t%%__tmp%d = %s %s ", tmpVarCounter, operation2, currentType);
		}
		generateCode(node->left);
		fprintf(file,", ");
		generateCode(node->right);
		fprintf(file, "\n");
	}
	tmpVarCounter++;
}

#endif