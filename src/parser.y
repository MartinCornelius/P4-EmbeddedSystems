%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include "include/symbolstructs.h"

    extern FILE *yyin; 

    enum types { symbol_enum = -1, input_enum, output_enum, int8_enum, int16_enum, uint8_enum, 
                 uint16_enum, float8_enum, float16_enum, bool_enum, char_enum, flexint_enum };

    int yylex();
    int yyerror(char *s);
    Symbol_Struct* findSymbol(char* name);
    void createToken(int type, char* name);
    void changeTokenVal(char* name, int f );

    int cOp(int x, char* op, int y);
    int logCop(int x, char* logop, int y);

    //for debugging skal fjernes senere
    void printTable();

    // Start of linked list
    Symbol_Struct handle;
    // Last element in list
    void *listHead;

%}

%union{ int val; int type; char* id; char* str; int boolean; }

%start prog

%token<val> VAL
%token<str> COP LOGOP
%token<id> ID
%token<type> TYPE
%token<boolean> BOOLVAL


%token DEFINE SETUP MAIN FUNC LARROW
        RARROW LBRA RBRA RPAR LPAR
        PLUS MINUS TIMES DIV SEMI 
        COMMA PRINT
%token ASSIGN WHILE IF ELSE


%type<boolean> compare comparelist boolexpr
%type<val> expr term factor

%%
prog          : defines setup mainloop funcs                                                        
              ;
defines       : define defines
              |
              ;
define        : DEFINE ID expr
              ;
setup         : SETUP LBRA vardecls funccalls RBRA
              ;
mainloop      : MAIN LBRA lines RBRA                                                
              ;
funcs         : func funcs
              |
              ;
func          : FUNC ID LPAR paramsdecl RPAR LBRA lines RBRA
              | FUNC ID LPAR paramsdecl RARROW paramlistdecl RPAR LBRA lines RBRA
              ;
paramsdecl    : paramlistdecl
              |
              ;
paramlistdecl : TYPE ID COMMA paramlistdecl                                 
              | TYPE ID                                                     
              ;
lines         : line SEMI lines
              | control lines
              |
              ;
line          : ID ASSIGN expr                                             { changeTokenVal($1, $3); }
              | ID LARROW expr                                             { changeTokenVal($1, $3); }
              | funccall
              | PRINT                                                      { printTable(); }
              |
              ;
control       : WHILE LPAR comparelist RPAR LBRA lines RBRA
              | IF LPAR comparelist RPAR LBRA lines RBRA elsechain
              ;
elsechain     : ELSE IF LPAR comparelist RPAR LBRA lines RBRA elsechain
              | ELSE LBRA lines RBRA
              |
              ;
vardecls      : vardecl SEMI vardecls
              |
              ;
vardecl       : TYPE ID                                                     { createToken($1, $2); }
              | TYPE ID ASSIGN expr                                         { createToken($1, $2); changeTokenVal($2, $4); }
              ;
funccalls     : funccall SEMI funccalls
              |
              ;
funccall      : ID LPAR paramscall RPAR
              | ID LPAR paramscall RARROW paramlistcall RPAR
              ;
paramscall    : paramlistcall
              |
              ;
paramlistcall : expr COMMA paramlistcall
              | expr
              ;
expr          : expr PLUS term                                              { $$ = $1 + $3; }                        
              | expr MINUS term                                             { $$ = $1 - $3; }
              | term                                                        { $$ = $1; }
              ;
term          : term TIMES factor                                           { $$ = $1 * $3; }
              | term DIV factor                                             { if($3 != 0){ $$ = $1 / $3; }else{ $$ = 0; } }
              | factor                                                      { $$ = $1; }
              ;
factor        : ID                                                          { Symbol_Struct *a = findSymbol($1); $$ = a->value; }
              | VAL                                                         { $$ = $1; }
              | LPAR expr RPAR                                              { $$ = $2; }
              ;
comparelist   : compare LOGOP comparelist                                   { $$ = logCop($1, $2, $3); }
              | compare                                                     { $$ = $1; }
              ;
compare       : boolexpr COP boolexpr                                       { $$ = cOp($1, $2, $3); }
              | ID                                                          { Symbol_Struct *a = findSymbol($1); $$ = a->value; }
              ;
boolexpr      : ID                                                          {}
              | VAL                                                         {}
              ;
%%

void main(int argc, char **argv)
{
    handle.next = NULL;
    
    listHead = (struct Symbol *)&handle;

    if (argc > 1)
      if (!(yyin = fopen(argv[1], "r")))
        perror("Error loading file\n");
    yyparse();
}

void printTable()
{
    Symbol_Struct *temp = &handle;
    while(temp->next != NULL)
    {
        temp = temp->next;
        printf("___________________\n");

        printf("Name: %s, Type: %d, Value: %d\n", temp->name, temp->type, temp->value);

        printf("___________________\n");
    }
}

void createToken(int type, char* name)
{
    if(findSymbol(name) == NULL){

        switch(type)
        {
            case -1:
                break;
            case input_enum:
                printf("yo3");
                break;
            case output_enum:
                printf("yo");
                break;
            case int8_enum:
                ; // is necessary!
                int8_Struct *newInt8Symbol = calloc(1, sizeof(int8_Struct));
                strcpy(newInt8Symbol->name, name);
                newInt8Symbol->type = int8_enum;
                ((int8_Struct *)listHead)->next = newInt8Symbol;
                listHead = newInt8Symbol;
                break;
            case float8_enum:
                ; // is still necessary!!
                float8_Struct *newFloat8Symbol = calloc(1, sizeof(float8_Struct));
                strcpy(newFloat8Symbol->name, name);
                newFloat8Symbol->type = float8_enum;
                ((float8_Struct *)listHead)->next = newFloat8Symbol;
                listHead = newFloat8Symbol;
                break;
            default:
                printf("Illegal type (defaulting as flexint)\n");
                flexint_Struct *newFlexIntSymbol = calloc(1, sizeof(flexint_Struct));
                strcpy(newFlexIntSymbol->name, name);
                newFlexIntSymbol->type = flexint_enum;
                ((flexint_Struct *)listHead)->next = newFlexIntSymbol;
                listHead = newFlexIntSymbol;
        }
        
    }else{
        printf("Declartion of two types of same name is not valid");
    }
}

/* void inputSymbolTable(int type)
{
    switch(type)
        {
            case -1:
                break;
            case input_enum:
                printf("yo3");
                break;
            case output_enum:
                printf("yo");
                break;
            case int8_enum:
                ; // is necessary!
                int8_Struct *newSymbol = calloc(1, sizeof(int8_Struct));
                strcpy(newSymbol->name, name);
                newSymbol->type = int8_enum;
                break;
            case float8_enum:
                ; // is still necessary!!
                float8_Struct *newSymbol = calloc(1, sizeof(float8_Struct));
                strcpy(newSymbol->name, name);
                newSymbol->type = float8_enum;
                
                break;
            default:
                printf("Illegal type\n");
        }
} */

void changeTokenVal(char* name, int val )
{
    Symbol_Struct* symbol = findSymbol(name);
    if(symbol != NULL)
    {
        symbol->value = val;
    }
}

Symbol_Struct* findSymbol(char* name)
{
    Symbol_Struct* temp = &handle;
    while(temp->next != NULL)
    {
        temp = temp->next;
        if(strcmp(temp->name, name) == 0)
        {
            return temp;
        }
    }
    return NULL;
}

int cOp(int x, char* cop, int y)
{
    if(strcmp(cop, "==") == 0)
    {
        return x == y;
    }else if(strcmp(cop, "<=") == 0)
    {
        return x <= y;
    }else if(strcmp(cop, "<") == 0)
    {
        return x < y;
    }else if(strcmp(cop, ">=") == 0)
    {
        return x >= y;
    }else if(strcmp(cop, ">") == 0)
    {
        return x > y;
    }else if(strcmp(cop, "!=") == 0)
    {
        return x != y;
    }
    printf("illegal cmpoperator");
    return false;
}

int logCop(int x, char* logop, int y)
{
  if(strcmp(logop, "&&") == 0)
  {
    return x && y;
  }else if(strcmp(logop, "||") == 0)
  {
    return x || y;
  }
  printf("illegal logoperator");
  return false;
}

int yyerror(char *s){
    printf("The error: %s", s);
    printTable();
    return 0;
}
