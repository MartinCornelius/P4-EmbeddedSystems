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

struct HashTables *symTable;

void loadParams(struct ast *node);
void loadLocalParams(struct ast *node);

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
		fprintf(file, "define void @setup() {\n");
		fprintf(file, "entry:\n");
		generateCode(node->left);
		fprintf(file, "\tret void\n");
		fprintf(file, "}\n\n");
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
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type);
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
				currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->right)->string).type);
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
		currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type);
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

		fprintf(file, "\n\tcall i32(i8*,...) @printf(i8* getelementptr([5 x i8], [5 x i8]* @pfmt, i32 0, i32 0), i32 %s%d", (casted == 0 ? "%__tmpGlobal_" : "%__castGlobal_"), globalVarCounter);
		generateCode(node->left);
		fprintf(file, ");\n");
		globalVarCounter++;
		tmpVarCounter++;
		break;

	/* Control structures */
	case WHILE:
		// Get current type
		if (node->left->left->type == ID)
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left->left)->string).type);
		else if (node->left->right->type == ID)
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left->right)->string).type);
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
		currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type);

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

		currentType = typeConverter(search.type);

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
		fprintf(file, "\n\tstore %s %%__tmp%d, %s* %%sc%d_", currentType, tmpVarCounter - 1, currentType, currentScope);
		generateCode(node->left);
		tmpVarCounter++;
		break;
	}
	/* Arithmetic */
	case PLUS:
		if (node->right->type == ID)
		{
			if (node->left->type == ID)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fadd %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmpVarCounter - 2);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmpVarCounter - 2);
				}
			}
			else if (node->left->type == VAL)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = add %s ", tmpVarCounter, currentType);
				generateCode(node->left);
				fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
			}
			else if (node->left->type == VALF)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = fadd %s ", tmpVarCounter, currentType);
				generateCode(node->left);
				fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
			}
			else
			{
				generateCode(node->left);
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fadd %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmpVarCounter - 2);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmpVarCounter - 2);
				}
			}
		}
		else if (node->left->type == ID)
		{
			if (node->right->type == ID)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fadd %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
			else if (node->right->type == VAL)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else if (node->right->type == VALF)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = fadd %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->right);
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fadd %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
		}
		else if (node->right->type != VAL)
		{
			generateCode(node->right);
			int tmp = tmpVarCounter - 1;
			if (node->left->type == VAL)
			{
				fprintf(file, "\t%%__tmp%d = add %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->left);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->left);
				fprintf(file, "\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmp);
			}
		}
		else if (node->left->type != VAL)
		{
			generateCode(node->left);
			int tmp = tmpVarCounter - 1;
			if (node->right->type == VAL)
			{
				fprintf(file, "\t%%__tmp%d = add %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->right);
				fprintf(file, "\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmp);
			}
		}
		else
		{
			if (currentType[0] == 'f')
			{
				fprintf(file, "\t%%__tmp%d = fadd %s ", tmpVarCounter, currentType);
			}
			else
			{
				fprintf(file, "\t%%__tmp%d = add %s ", tmpVarCounter, currentType);
			}
			generateCode(node->right);
			fprintf(file, ", ");
			generateCode(node->left);
			fprintf(file, "\n");
		}
		tmpVarCounter++;
		break;
	case MINUS:
		if (node->right->type == ID)
		{
			if (node->left->type == ID)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fsub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
			else if (node->left->type == VAL)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = sub %s ", tmpVarCounter, currentType);
				generateCode(node->left);
				fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
			}
			else if (node->left->type == VALF)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = fsub %s ", tmpVarCounter, currentType);
				generateCode(node->left);
				fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
			}
			else
			{
				generateCode(node->left);
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fsub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
			}
		}
		else if (node->left->type == ID)
		{
			if (node->right->type == ID)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				if (currentType[0] = 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fsub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
			else if (node->right->type == VAL)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else if (node->right->type == VALF)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = fsub %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->right);
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fsub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
		}
		else if (node->right->type != VAL)
		{
			generateCode(node->right);
			int tmp = tmpVarCounter - 1;
			if (node->left->type == VAL)
			{
				fprintf(file, "\t%%__tmp%d = sub %s ", tmpVarCounter, currentType);
				generateCode(node->left);
				fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
			}
			else
			{
				generateCode(node->left);
				fprintf(file, "\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmp);
			}
		}
		else if (node->left->type != VAL)
		{
			generateCode(node->left);
			int tmp = tmpVarCounter - 1;
			if (node->right->type == VAL)
			{
				fprintf(file, "\t%%__tmp%d = sub %s ", tmpVarCounter, currentType);
				fprintf(file, "%%__tmp%d, ", tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->right);
				fprintf(file, "\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmp, tmpVarCounter - 1);
			}
		}
		else
		{
			fprintf(file, "\t%%__tmp%d = sub %s ", tmpVarCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", ");
			generateCode(node->right);
			fprintf(file, "\n");
		}
		tmpVarCounter++;
		break;
	case TIMES:
		if (node->right->type == ID)
		{
			if (node->left->type == ID)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fmul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
			else if (node->left->type == VAL)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = mul %s ", tmpVarCounter, currentType);
				generateCode(node->left);
				fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
			}
			else if (node->left->type == VALF)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = fmul %s ", tmpVarCounter, currentType);
				generateCode(node->left);
				fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
			}
			else
			{
				generateCode(node->left);
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fmul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
		}
		else if (node->left->type == ID)
		{
			if (node->right->type == ID)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fmul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
			else if (node->right->type == VAL)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else if (node->right->type == VALF)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = fmul %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->right);
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
			}
		}
		else if (node->right->type != VAL)
		{
			generateCode(node->right);
			int tmp = tmpVarCounter - 1;
			if (node->left->type == VAL)
			{
				fprintf(file, "\t%%__tmp%d = mul %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->left);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->left);
				fprintf(file, "\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmp);
			}
		}
		else if (node->left->type != VAL)
		{
			generateCode(node->left);
			int tmp = tmpVarCounter - 1;
			if (node->right->type == VAL)
			{
				fprintf(file, "\t%%__tmp%d = mul %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->right);
				fprintf(file, "\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmp);
			}
		}
		else
		{
			fprintf(file, "\t%%__tmp%d = mul %s ", tmpVarCounter, currentType);
			generateCode(node->right);
			fprintf(file, ", ");
			generateCode(node->left);
			fprintf(file, "\n");
		}
		tmpVarCounter++;
		break;
	case DIV:
		if (node->right->type == ID)
		{
			if (node->left->type == ID)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
			else if (node->left->type == VAL)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = sdiv %s ", tmpVarCounter, currentType);
				generateCode(node->left);
				fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
			}
			else if (node->left->type == VALF)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = fdiv %s ", tmpVarCounter, currentType);
				generateCode(node->left);
				fprintf(file, ", %%__tmp%d\n", tmpVarCounter - 1);
			}
			else
			{
				generateCode(node->left);
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
		}
		else if (node->left->type == ID)
		{
			if (node->right->type == ID)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				tmpVarCounter++;
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
			else if (node->right->type == VAL)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else if (node->right->type == VALF)
			{
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				// Make addition operation
				fprintf(file, "\n\t%%__tmp%d = fdiv %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->right);
				// Load variable
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->left);
				tmpVarCounter++;
				if (currentType[0] == 'f')
				{
					fprintf(file, "\n\t%%__tmp%d = fdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
				else
				{
					fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 2, tmpVarCounter - 1);
				}
			}
		}
		else if (node->right->type != VAL)
		{
			generateCode(node->right);
			int tmp = tmpVarCounter - 1;
			if (node->left->type == VAL)
			{
				fprintf(file, "\t%%__tmp%d = sdiv %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->left);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->left);
				fprintf(file, "\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmp);
			}
		}
		else if (node->left->type != VAL)
		{
			generateCode(node->left);
			int tmp = tmpVarCounter - 1;
			if (node->right->type == VAL)
			{
				fprintf(file, "\t%%__tmp%d = sdiv %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter - 1);
				generateCode(node->right);
				fprintf(file, "\n");
			}
			else
			{
				generateCode(node->right);
				fprintf(file, "\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter - 1, tmp);
			}
		}
		else
		{
			if (currentType[0] == 'f')
			{
				fprintf(file, "\t%%__tmp%d = fdiv %s ", tmpVarCounter, currentType);
			}
			else
			{
				fprintf(file, "\t%%__tmp%d = sdiv %s ", tmpVarCounter, currentType);
			}
			generateCode(node->left);
			fprintf(file, ", ");
			generateCode(node->right);
			fprintf(file, "\n");
		}
		tmpVarCounter++;
		break;

	/* Compare operators */
	case COPLE:
		if (node->left->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->left);
			if (node->right->type == ID)
			{
				tmpVarCounter++;
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				fprintf(file, "\n\t%%cmp%d = icmp sle %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter - 1, tmpVarCounter);
			}
			else
			{
				fprintf(file, "\n\t%%cmp%d = icmp sle %s %%__tmp%d, ", cmpCounter, currentType, tmpVarCounter);
				generateCode(node->right);
			}
			tmpVarCounter++;
		}
		else if (node->right->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->right);
			fprintf(file, "\n\t%%cmp%d = icmp sle %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", %%__tmp%d", tmpVarCounter);
			tmpVarCounter++;
		}
		else
		{
			fprintf(file, "\n\t%%cmp%d = icmp sle %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", ");
			generateCode(node->right);
		}
		fprintf(file, "\n");
		break;
	case COPGE:
		if (node->left->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->left);
			if (node->right->type == ID)
			{
				tmpVarCounter++;
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				fprintf(file, "\n\t%%cmp%d = icmp sge %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter - 1, tmpVarCounter);
			}
			else
			{
				fprintf(file, "\n\t%%cmp%d = icmp sge %s %%__tmp%d, ", cmpCounter, currentType, tmpVarCounter);
				generateCode(node->right);
			}
			tmpVarCounter++;
		}
		else if (node->right->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->right);
			fprintf(file, "\n\t%%cmp%d = icmp sge %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", %%__tmp%d", tmpVarCounter);
			tmpVarCounter++;
		}
		else
		{
			fprintf(file, "\n\t%%cmp%d = icmp sge %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", ");
			generateCode(node->right);
		}
		fprintf(file, "\n");
		break;
	case COPL:
		if (node->left->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->left);
			if (node->right->type == ID)
			{
				tmpVarCounter++;
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				fprintf(file, "\n\t%%cmp%d = icmp slt %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter - 1, tmpVarCounter);
			}
			else
			{
				fprintf(file, "\n\t%%cmp%d = icmp slt %s %%__tmp%d, ", cmpCounter, currentType, tmpVarCounter);
				generateCode(node->right);
			}
			tmpVarCounter++;
		}
		else if (node->right->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->right);
			fprintf(file, "\n\t%%cmp%d = icmp slt %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", %%__tmp%d", tmpVarCounter);
			tmpVarCounter++;
		}
		else
		{
			fprintf(file, "\n\t%%cmp%d = icmp slt %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", ");
			generateCode(node->right);
		}
		fprintf(file, "\n");
		break;
	case COPG:
		if (node->left->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->left);
			if (node->right->type == ID)
			{
				tmpVarCounter++;
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				fprintf(file, "\n\t%%cmp%d = icmp sgt %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter - 1, tmpVarCounter);
			}
			else
			{
				fprintf(file, "\n\t%%cmp%d = icmp sgt %s %%__tmp%d, ", cmpCounter, currentType, tmpVarCounter);
				generateCode(node->right);
			}
			tmpVarCounter++;
		}
		else if (node->right->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->right);
			fprintf(file, "\n\t%%cmp%d = icmp sgt %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", %%__tmp%d", tmpVarCounter);
			tmpVarCounter++;
		}
		else
		{
			fprintf(file, "\n\t%%cmp%d = icmp sgt %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", ");
			generateCode(node->right);
		}
		fprintf(file, "\n");
		break;
	case COPEQ:
		if (node->left->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->left);
			if (node->right->type == ID)
			{
				tmpVarCounter++;
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				fprintf(file, "\n\t%%cmp%d = icmp eq %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter - 1, tmpVarCounter);
			}
			else
			{
				fprintf(file, "\n\t%%cmp%d = icmp eq %s %%__tmp%d, ", cmpCounter, currentType, tmpVarCounter);
				generateCode(node->right);
			}
			tmpVarCounter++;
		}
		else if (node->right->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->right);
			fprintf(file, "\n\t%%cmp%d = icmp eq %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", %%__tmp%d", tmpVarCounter);
			tmpVarCounter++;
		}
		else
		{
			fprintf(file, "\n\t%%cmp%d = icmp eq %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", ");
			generateCode(node->right);
		}
		fprintf(file, "\n");
		break;
	case COPNEQ:
		if (node->left->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->left);
			if (node->right->type == ID)
			{
				tmpVarCounter++;
				fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
				generateCode(node->right);
				fprintf(file, "\n\t%%cmp%d = icmp ne %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter - 1, tmpVarCounter);
			}
			else
			{
				fprintf(file, "\n\t%%cmp%d = icmp ne %s %%__tmp%d, ", cmpCounter, currentType, tmpVarCounter);
				generateCode(node->right);
			}
			tmpVarCounter++;
		}
		else if (node->right->type == ID)
		{
			// Load variable
			fprintf(file, "\n\t%%__tmp%d = load %s, %s* %%sc%d_", tmpVarCounter, currentType, currentType, currentScope);
			generateCode(node->right);
			fprintf(file, "\n\t%%cmp%d = icmp ne %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", %%__tmp%d", tmpVarCounter);
			tmpVarCounter++;
		}
		else
		{
			fprintf(file, "\n\t%%cmp%d = icmp ne %s ", cmpCounter, currentType);
			generateCode(node->left);
			fprintf(file, ", ");
			generateCode(node->right);
		}
		fprintf(file, "\n");
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
	fprintf(file, "@pfmt = constant [5 x i8] c\"%%ld\\0A\\00\"\n");
	fprintf(file, "declare i32 @printf(i8*,...)\n\n");

	// Global variables
	for (int i = 0; i < symTable->hTable[0]->size; i++)
	{
		if (symTable->hTable[0]->items[i] != NULL)
		{
			int itemType = searchSymbol(symTable->hTable[0], symTable->hTable[0]->items[i]->name).type;
			// Needs converter
			printf("itemType: %i\n", itemType);
			char *floatOrInt = ((typeConverter(itemType) == "half") || (typeConverter(itemType) == "float") || (typeConverter(itemType) == "double")) ? "0.0" : "0";
			fprintf(file, "@%s = global %s %s\n", symTable->hTable[0]->items[i]->name, typeConverter(itemType), floatOrInt);
		}
	}
	fprintf(file, "\n");

	generateCode(node);
	fprintf(file, "\ndefine i32 @main() {\n");
	fprintf(file, "entry:\n");
	fprintf(file, "\tcall void @s`etup()\n");
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
		printf("type %s\n", currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type));
		currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type);
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
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->right)->string).type);
			fprintf(file, "\t%s", currentScope);
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
		currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->left)->string).type);
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
			currentType = typeConverter(searchSymbol(symTable->hTable[currentScope], ((struct astLeafStr *)node->right)->string).type);
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

#endif