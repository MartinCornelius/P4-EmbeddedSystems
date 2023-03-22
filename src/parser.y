%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>

    #define MAX 40

    extern FILE *yyin;

    int yylex();
    int yyerror(char *s);
    int findIndex(char* name);
    void createToken(char* type, char* name);
    void changeTokenVal(char* name, int f );

    int cOp(int x, char* op, int y);
    int logCop(int x, char* logop, int y);

    //for debugging skal fjernes senere
    void printTable();

    struct Symbol{
        char type[9];
        char name[32];
        int valueF;
    };
    typedef struct Symbol Symbol_Struct;

    Symbol_Struct* getToken(char* name);
    
    int amount = 0;

    Symbol_Struct *symbolTable[MAX];
%}

%union{ int val; char* type; char* id; char* str; int boolean; }

%start prog

%token<val> VAL
%token<str> COP LOGOP
%token<id> ID
%token<type> TYPE
%token<boolean> BOOLVAL


%token DEFINE SETUP MAIN FUNC LARROW
        RARROW LBRA RBRA RPAR LPAR
        PLUS MINUS TIMES DIV COLON
        QUEST SEMI COMMA
%token ASSIGN WHILE IF ELSE


%type<boolean> compare comparelist boolexpr
//ved ikke helt om factor er en value lol
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
factor        : ID                                                          { Symbol_Struct *a = getToken($1); $$ = a->valueF; }
              | VAL                                                         { $$ = $1; }
              | LPAR expr RPAR                                              { $$ = $2; }
              ;
comparelist   : compare LOGOP comparelist                                   { $$ = logCop($1, $2, $3); }
              | compare                                                     { $$ = $1; }
              ;
compare       : boolexpr COP boolexpr                                       { $$ = cOp($1, $2, $3); }
              | ID                                                          { Symbol_Struct *a = getToken($1); $$ = a->valueF; }
              ;
boolexpr      : ID                                                          {}
              | VAL                                                         {}
              ;
%%

void main(int argc, char **argv)
{
    if (argc > 1)
      if (!(yyin = fopen(argv[1], "r")))
        perror("Error loading file\n");
    yyparse();
}

void printTable()
{
    printf("\n_____________________________________________\n");
    for(int i = 0; i < amount; i++)
    {
        printf("\n_______________________\ntable: %d\nname: %s ", i, symbolTable[i]->name);
        printf("type: %s ", symbolTable[i]->type);
        printf("valF: %d \n______________\n", symbolTable[i]->valueF);
    }
}

void createToken(char* type, char* name)
{
    if(amount != MAX /* && findIndex(newName) == -1 */){
        symbolTable[amount] = malloc(sizeof(Symbol_Struct));
        strcpy(symbolTable[amount]->type, type);
        strcpy(symbolTable[amount]->name, name);
        symbolTable[amount]->valueF = 0;
        amount++;
    }else if(findIndex(name) != -1){
        printf("Declartion of two types of same name is not valid");
    }else{
        printf("No more availible space");
    }
}

void changeTokenVal(char* name, int f )
{
    int i = findIndex(name);
    if(i != -1)
    {
        symbolTable[i]->valueF = f;
    }
    
}

int findIndex(char* name)
{
    int i = 0;
    int found = 0;
    while(i < MAX)
    {
        if(strcmp(symbolTable[i]->name, name) == 0)
        {
            return i;
        }else{
            i++;
        } 
    }
    if(i == MAX)
    {
        printf("Error,\"%s\" was not in the table", name);
        return -1;
    }
    return i;
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

Symbol_Struct* getToken(char* name)
{
    int i = findIndex(name);
    if(i != -1)
    {
        return symbolTable[i];
    }
    printf("No such symbol exists");
    Symbol_Struct *error;
    return error;
}

int yyerror(char *s){
    printf("%s", s);


    printTable();
    return 0;
}
