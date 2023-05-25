#ifndef TYPE2TEXT_H
#define TYPE2TEXT_H

#include <stdio.h>
#include "symbol_types.h"
#include "../parser.tab.h"

char *printType(int type)
{
  static char result[30];

  switch (type)
  {
  case ROOT:
    sprintf(result, "root");
    break;
  case SETUP:
    sprintf(result, "setup");
    break;
  case MAIN:
    sprintf(result, "mainloop");
    break;
  case FUNCS:
    sprintf(result, "funcs");
    break;
  case FUNC:
    sprintf(result, "func");
    break;
  case FUNCCALL:
    sprintf(result, "funccall");
    break;
  case PARAMS:
    sprintf(result, "params");
    break;
  case PARAM:
    sprintf(result, "param");
    break;
  case LINES:
    sprintf(result, "lines");
    break;
  case DECL:
    sprintf(result, "vardecl");
    break;
  case CONTROL:
    sprintf(result, "control");
    break;
  case PRINT:
    sprintf(result, "print");
    break;

  /* Control structures */
  case WHILE:
    sprintf(result, "while");
    break;
  case IF:
    sprintf(result, "if");
    break;

  case ASSIGN:
    sprintf(result, "assign");
    break;

  /* Logical operators */
  case LOGOR:
    sprintf(result, "lor");
    break;
  case LOGAND:
    sprintf(result, "land");
    break;

  /* Compare operators */
  case COPGE:
    sprintf(result, "greater eq");
    break;
  case COPLE:
    sprintf(result, "less eq");
    break;
  case COPEQ:
    sprintf(result, "eq");
    break;
  case COPNEQ:
    sprintf(result, "not eq");
    break;
  case COPL:
    sprintf(result, "less");
    break;
  case COPG:
    sprintf(result, "greater");
    break;

  /* Arithmetic operations */
  case PLUS:
    sprintf(result, "plus");
    break;
  case MINUS:
    sprintf(result, "minus");
    break;
  case TIMES:
    sprintf(result, "times");
    break;
  case DIV:
    sprintf(result, "div");
    break;

  /* Expression */
  case TERM:
    sprintf(result, "term");
    break;
  case FACTOR:
    sprintf(result, "factor");
    break;

  /* Types */
  case ID:
    sprintf(result, "identifier");
    break;
  case VAL:
    sprintf(result, "integer");
    break;
  case VALF:
    sprintf(result, "float");
    break;

  /* Misc */
  case EMPTY:
    sprintf(result, "empty");
    break;
  default:
    sprintf(result, "not implemented");
    break;
  }

  return result;
}

/* Converts enum type to c-like type */
char *getCustomType(int varType)
{
  char *result;

  switch (varType)
  {
  case int8_enum:
    result = "int8";
    break;
  case int16_enum:
    result = "int16";
    break;
  case int32_enum:
    result = "int32";
    break;
  case uint8_enum:
    result = "uint8";
    break;
  case uint16_enum:
    result = "uint16";
    break;
  case uint32_enum:
    result = "uint32";
    break;
  case float16_enum:
    result = "float16";
    break;
  case float32_enum:
    result = "float32";
    break;
  case float64_enum:
    result = "float64";
    break;
  default:
    result = "type not implemented";
    break;
  }

  return result;
}

/* Converts from enum type to llvm ir type */
char *typeConverter(int type, int debug)
{
  char *result;
  switch (type)
  {
  case int8_enum:
    result = "i8";
    break;
  case int16_enum:
    result = "i16";
    break;
  case int32_enum:
    result = "i32";
    break;
  case uint8_enum:
    result = "i8";
    break;
  case uint16_enum:
    result = "i16";
    break;
  case uint32_enum:
    result = "i32";
    break;
  case float16_enum:
    result = "half";
    break;
  case float32_enum:
    result = "float";
    break;
  case float64_enum:
    result = "double";
    break;
  default:
    printf("type convertion failed\n");
    break;
  }
  return result;
}

#endif