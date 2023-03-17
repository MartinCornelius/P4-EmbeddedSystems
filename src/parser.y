%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #define MAX 40
    int yylex();
    int yyerror(char *s);
    int findIndex(char* name);
    void createToken(char* type, char* name);
    void changeTokenVal(char* name, char* s, int f );

    //for debugging skal fjernes senere
    void printTable();

    struct Token{
        char* type;
        char* name;
        char* valueS;
        int valueF;
    };
    typedef struct Token Token_Struct;
    
    int amount = 0;

    Token_Struct symbolTable[MAX];

%}

%union{ int val; char* type; char* id; char* str; int boolean; }

%start prog

%token<val> VAL
%token<str> STRING
%token<id> ID
%token<type> TYPE


%token DEFINE SETUP MAIN FUNC LARROW
        RARROW LBRA RBRA RPAR LPAR
        PLUS MINUS TIMES DIV COLON
        QUEST SEMI COMMA
%token ASSIGN WHILE IF ELSE COP LOGOP


%type<boolean> compare
//ved ikke helt om factor er en value lol
%type<val> expr term factor

%%
prog          : defines funcs setup mainloop                                                              
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
vardecl       : TYPE ID                                                     { createToken($1, $2); }
              | TYPE ID ASSIGN expr                                         { createToken($1, $2); }
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
              | comparelist QUEST expr COLON expr
              | term                                                        { $$ = $1; }
              ;
term          : term TIMES factor                                           { $$ = $1 * $3; }
              | term DIV factor                                             { if($3 != 0){ $$ = $1 / $3; }else{ $$ = 0; } }
              | factor                                                      { $$ = $1 }
              ;
factor        : ID                                                          { int i = findIndex($1); $$ = symbolTable[i].valueF;}
              | VAL                                                         { $$ = $1; }
              | STRING
              | LPAR expr RPAR                                              
              ;
comparelist   : compare LOGOP comparelist
              | compare
              ;
compare       : boolexpr COP boolexpr                                       { $$ = 1; }
              | ID                                                          { $$ = 1; }
              ;
boolexpr      : ID                                                                                                        
              | VAL                                                                                                              
              | LPAR boolexpr RPAR 
              ;
%%

int main()
{

    yyparse();

    return 1;
}

void printTable()
{
    printf("yo + %d", amount);
    for(int i = 0; i < amount; i++)
    {
        printf("\n_______________________\nname: %s ", symbolTable[i].name);
        printf("type: %s ", symbolTable[i].type);
        printf("valS: %s ", symbolTable[i].valueS);
        printf("valF: %d\n______________\n", symbolTable[i].valueF);
    }
}

void createToken(char* type, char* name)
{

    int count = 0;
    int i = 0;
    while(type[count] != ' '/* && type[count] != '\0' */)
    {
        count++;
    }
    char newType[count + 1];
    for(i = 0; i < count; i++){
        newType[i] = type[i];
    }
    newType[i] = '\0';


    count = 0;
    while(name[count] != ' ')
    {
        count++;
    }
    char newName[count + 1];
    for(i = 0; i < count; i++){
        newName[i] = name[i];
    }
    newName[i] = '\0';

   

    if(amount != MAX){
        Token_Struct newStruct;
        newStruct.type = newType;
        newStruct.name = newName;
        newStruct.valueS = "";
        newStruct.valueF = 0;
        symbolTable[amount] = newStruct;
        amount++;
    }else{
        printf("No more availible space");
    }
    printf("\n");
    printTable();
    printf("\n");
}

void changeTokenVal(char* name, char* s, int f )
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
        if(strcmp(symbolTable[i].name, name))
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

    printTable();
    return 0;
}