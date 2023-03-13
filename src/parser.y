%{
    #include <stdio.h>
    #include <stdlib.h>
    #define MAX 40
    int yylex();
    int yyerror(char *s);
    int findIndex(char* name);
    void createToken(char* type, char* name);
    void changeTokenVal(char* name, char* s, float f );



    //for debugging skal fjernes senere
    void printTable();

    struct Token{
        char* type;
        char* name;
        char* valueS;
        float valueF;
    };
    typedef struct Token Token_Struct;
    
    int amount = 0;

    Token_Struct symbolTable[MAX];

%}

%union{ float val; char* type; char* id; char* str; int boolean; }

%start prog

%token<val> VAL
%token<str> STRING
%token<id> ID
%token<type> TYPE


%token DEFINE SETUP MAIN FUNC LARROW RARROW LBRA RBRA RPAR LPAR PLUS MINUS TIMES DIV COLON QUEST SEMI COMMA IMPORT HEADERF
%token ASSIGN WHILE IF ELSE COP LOGOP


%type<boolean> compare

%%
prog          : imports defines funcs setup mainloop                                              
              ;
imports       : import imports
              |
              ;
import        : IMPORT HEADERF
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
line          : ID ASSIGN expr
              | ID LARROW expr
              | expr
              | funccall
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
vardecl       : TYPE ID                                                     { createToken($1, $2); printTable(); }
              | TYPE ID ASSIGN expr                                         { createToken($1, $2); printTable(); }
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
expr          : expr PLUS term
              | expr MINUS term
              | comparelist QUEST expr COLON expr
              | term
              ;
term          : term TIMES factor
              | term DIV factor
              | factor
              ;
factor        : ID                                                  
              | VAL
              | STRING
              | LPAR expr RPAR                          
              ;
comparelist   : compare LOGOP comparelist
              | compare
              ;
compare       : expr COP expr                                   {$$ = 1}
              | ID                                              {$$ = 1}
              ;
%%

int main()
{

    yyparse();

    return 1;
}

void printTable()
{
    for(int i = 0; i < amount; i++)
    {
        printf("name: %s type: %s valS: %s valF: %f\n", symbolTable[i].name, symbolTable[i].type, symbolTable[i].valueS, symbolTable[i].valueF);
    }
}

void createToken(char* type, char* name)
{
    if(amount != MAX){
        Token_Struct newStruct;
        newStruct.type = type;
        newStruct.name = name;
        symbolTable[amount] = newStruct;
        amount++;
    }else{
        printf("No more availible space");
    }
}

void changeTokenVal(char* name, char* s, float f )
{
    int i = findIndex(name);

    symbolTable[i].valueF = f;
    symbolTable[i].valueS = s;

}

int findIndex(char* name)
{
    int i = 0;
    int found = 0;
    while(i < MAX && !found)
    {
        if(symbolTable[i].name = name)
        {
            found = 1;
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

Token_Struct getToken(char* name)
{
    int i = findIndex(name);
    if(i != -1)
    {
        return symbolTable[i];
    }
    printf("No such symbol exists");
    Token_Struct error;
    return error;
}

int yyerror(char *s){
    printf("%s", s);
    return 0;
}