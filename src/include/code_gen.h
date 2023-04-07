#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ifCounter = 1;
int cmpCounter = 1;
int whileCounter = 1;
int tmpVarCounter = 1;
int globalVarCounter = 1;
char *currentVarName;
char *currentType;
char tmpVarName[30];

void generateCode(struct ast *node)
{
    if (node == NULL)
        return;    

    switch (node->type)
    {
    case ROOT:
        generateCode(node->left);
        generateCode(node->right);
        break;
    case SETUP:
        fprintf(file, "define void @setup() {\n");
        fprintf(file, "entry:\n");
        generateCode(node->left);
        fprintf(file, "\tret void\n");
        fprintf(file, "}\n\n");
        break;
    case MAIN:
        fprintf(file, "define void @mainloop() {\n");
        fprintf(file, "entry:\n");
        generateCode(node->left);
        fprintf(file, "\tret void\n");
        fprintf(file, "}\n\n");
        break;
    case LINES:
        generateCode(node->left);
        fprintf(file, "\n");
        generateCode(node->right);
        break;
    case PRINT:
        int casted = 0;
        currentType = typeConverter(searchSymbol(hTable, ((struct astLeafStr *)node->left)->string));
        fprintf(file, "\t%%__tmpGlobal_%d", globalVarCounter);
        generateCode(node->left);
        fprintf(file, " = load %s, %s* @", currentType, currentType);
        generateCode(node->left);

        if(strcmp(currentType, "i32") != 0)
        {
          // Check if signed
          char *checksigned = getCustomType(searchSymbol(hTable, ((struct astLeafStr *)node->left)->string));
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
          currentType = typeConverter(searchSymbol(hTable, ((struct astLeafStr *)node->left->left)->string));
        else if (node->left->right->type == ID)
          currentType = typeConverter(searchSymbol(hTable, ((struct astLeafStr *)node->left->right)->string));
        else 
          currentType = "i32";

        fprintf(file, "br label %%while%d.cond\n\n", whileCounter);

        // While condition
        fprintf(file, "while%d.cond:\n", whileCounter);
        generateCode(node->left);
        fprintf(file, "\tbr i1 %%cmp%d, label %%while%d.body, label %%while%d.end\n", cmpCounter, whileCounter, whileCounter);

        // While body
        fprintf(file, "while%d.body:\n", whileCounter);
        generateCode(node->right);
        fprintf(file, "\tbr label %%while%d.cond\n", whileCounter);

        // While end
        fprintf(file, "while%d.end:\n", whileCounter);

        whileCounter++;
        cmpCounter++;
        break;

    case DECL:
        if (node->right == NULL)
          return;
        else
          generateCode(node->right);
        break;

    case ASSIGN:
        currentType = typeConverter(searchSymbol(hTable, ((struct astLeafStr *)node->left)->string));
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
        else
        {
          generateCode(node->right);
        }
        fprintf(file, "\n\tstore %s %%__tmp%d, %s* @", currentType, tmpVarCounter-1, currentType);
        generateCode(node->left);
        tmpVarCounter++;
        break;

    /* Arithmetic */
    case PLUS:
        if (node->right->type == ID)
        {
          if (node->left->type == ID)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-1, tmpVarCounter-2);
          }
          else if (node->left->type == VAL)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = add %s ", tmpVarCounter, currentType);
            generateCode(node->left);
            fprintf(file, ", %%__tmp%d\n", tmpVarCounter-1);
          }
          else 
          {
            generateCode(node->left);
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-1, tmpVarCounter-2);
          }
        }
        else if (node->left->type == ID)
        {
          if (node->right->type == ID)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
          else if (node->right->type == VAL)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter-1);
            generateCode(node->right);
            fprintf(file, "\n");
          }
          else 
          {
            generateCode(node->right);
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            fprintf(file, "\n\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
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
                fprintf(file, "\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-1, tmp);
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
                fprintf(file, "\t%%__tmp%d = add %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-1, tmp);
            }
        }
        else
        {
            fprintf(file, "\t%%__tmp%d = add %s ", tmpVarCounter, currentType);
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
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
          else if (node->left->type == VAL)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = sub %s ", tmpVarCounter, currentType);
            generateCode(node->left);
            fprintf(file, ", %%__tmp%d\n", tmpVarCounter-1);
          }
          else 
          {
            generateCode(node->left);
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
        }
        else if (node->left->type == ID)
        {
          if (node->right->type == ID)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
          else if (node->right->type == VAL)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter-1);
            generateCode(node->right);
            fprintf(file, "\n");
          }
          else 
          {
            generateCode(node->right);
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            fprintf(file, "\n\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
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
                fprintf(file, "\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-1, tmp);
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
                fprintf(file, "\t%%__tmp%d = sub %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmp, tmpVarCounter-1);
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
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
          else if (node->left->type == VAL)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = mul %s ", tmpVarCounter, currentType);
            generateCode(node->left);
            fprintf(file, ", %%__tmp%d\n", tmpVarCounter-1);
          }
          else 
          {
            generateCode(node->left);
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
        }
        else if (node->left->type == ID)
        {
          if (node->right->type == ID)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
          else if (node->right->type == VAL)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter-1);
            generateCode(node->right);
            fprintf(file, "\n");
          }
          else 
          {
            generateCode(node->right);
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            fprintf(file, "\n\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
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
                fprintf(file, "\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-1, tmp);
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
                fprintf(file, "\t%%__tmp%d = mul %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-1, tmp);
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
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
          else if (node->left->type == VAL)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = sdiv %s ", tmpVarCounter, currentType);
            generateCode(node->left);
            fprintf(file, ", %%__tmp%d\n", tmpVarCounter-1);
          }
          else 
          {
            generateCode(node->left);
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
        }
        else if (node->left->type == ID)
        {
          if (node->right->type == ID)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->right);
            tmpVarCounter++;
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
          }
          else if (node->right->type == VAL)
          {
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            // Make addition operation
            fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, ", tmpVarCounter, currentType, tmpVarCounter-1);
            generateCode(node->right);
            fprintf(file, "\n");
          }
          else 
          {
            generateCode(node->right);
            // Load variable
            fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
            generateCode(node->left);
            tmpVarCounter++;
            fprintf(file, "\n\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-2, tmpVarCounter-1);
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
                fprintf(file, "\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-1, tmp);
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
                fprintf(file, "\t%%__tmp%d = sdiv %s %%__tmp%d, %%__tmp%d\n", tmpVarCounter, currentType, tmpVarCounter-1, tmp);
            }
        }
        else
        {
            fprintf(file, "\t%%__tmp%d = sdiv %s ", tmpVarCounter, currentType);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
        generateCode(node->left);
        if (node->right->type == ID)
        {
          tmpVarCounter++;
          fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
          generateCode(node->right);
          fprintf(file, "\n\t%%cmp%d = icmp sle %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter-1, tmpVarCounter);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
        generateCode(node->left);
        if (node->right->type == ID)
        {
          tmpVarCounter++;
          fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
          generateCode(node->right);
          fprintf(file, "\n\t%%cmp%d = icmp sge %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter-1, tmpVarCounter);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
        generateCode(node->left);
        if (node->right->type == ID)
        {
          tmpVarCounter++;
          fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
          generateCode(node->right);
          fprintf(file, "\n\t%%cmp%d = icmp slt %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter-1, tmpVarCounter);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
        generateCode(node->left);
        if (node->right->type == ID)
        {
          tmpVarCounter++;
          fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
          generateCode(node->right);
          fprintf(file, "\n\t%%cmp%d = icmp sgt %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter-1, tmpVarCounter);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
        generateCode(node->left);
        if (node->right->type == ID)
        {
          tmpVarCounter++;
          fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
          generateCode(node->right);
          fprintf(file, "\n\t%%cmp%d = icmp eq %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter-1, tmpVarCounter);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
        generateCode(node->left);
        if (node->right->type == ID)
        {
          tmpVarCounter++;
          fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
          generateCode(node->right);
          fprintf(file, "\n\t%%cmp%d = icmp ne %s %%__tmp%d, %%__tmp%d", cmpCounter, currentType, tmpVarCounter-1, tmpVarCounter);
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
        fprintf(file, "\n\t%%__tmp%d = load %s, %s* @", tmpVarCounter, currentType, currentType);
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

  // Global variables
  for (int i = 0; i < hTable->size; i++)
  {
    if (hTable->items[i] != NULL)
    {
      int itemType = searchSymbol(hTable, hTable->items[i]->key);
      // Needs converter
      fprintf(file, "@%s = global %s 0\n", hTable->items[i]->key, typeConverter(itemType));
    }
  }
  fprintf(file, "\n");

  generateCode(node);
  fprintf(file, "\ndefine i32 @main() {\n");
  fprintf(file, "entry:\n");
  fprintf(file, "\tcall void @setup()\n");
  fprintf(file, "\tcall void @mainloop()\n");
  fprintf(file, "\tret i32 0\n");
  fprintf(file, "}\n\n");
}

#endif
