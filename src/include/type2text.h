#ifndef TYPE2TEXT_H
#define TYPE2TEXT_H

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
  case LINES:
    sprintf(result, "lines");
    break;
  case CONTROL:
    sprintf(result, "control");
    break;

  /* Control structures */
  case WHILE:
    sprintf(result, "while");
    break;
  case IF:
    sprintf(result, "if");
    break;
  case ELSEIF:
    sprintf(result, "elseif");
    break;
  case ELSE:
    sprintf(result, "else");
    break;
  case IFELSECHAIN:
    sprintf(result, "if chain");
    break;
  case ELSECHAIN:
    sprintf(result, "else chain");
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

#endif
