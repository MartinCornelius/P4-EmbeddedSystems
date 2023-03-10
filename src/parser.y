%{
    #include <stdio.h>
    #include <stdlib.h>
    int yylex();
    int yyerror(char *s);
%}

%union{ int val; char* id; }

%start prog

%token<val> VAL
%token<id> ID

%token DEFINE SETUP MAIN FUNC LARROW RARROW LBRA RBRA RPAR LPAR PLUS MINUS TIMES DIV COLON QUEST SEMI COMMA IMPORT HEADERF
%token TYPE ASSIGN WHILE IF ELSE COP

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
setup         : SETUP LBRA funccalls vardecls RBRA
              ;
mainloop      : MAIN LBRA lines RBRA
              ;
funcs         : func funcs
              |
              ;
func          : FUNC ID LPAR paramsdecl RPAR LBRA lines RBRA
              | FUNC ID LPAR paramsdecl RARROW paramlistdecl RPAR LBRA lines returns RBRA
              ;
returns       : return SEMI returns
              | return SEMI
              ;
return        : ID LARROW expr SEMI
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
              | expr
              | funccall
              ;
control       : WHILE LPAR compare RPAR LBRA lines RBRA
              | IF LPAR compare RPAR LBRA lines RBRA elsechain
              ;
elsechain     : ELSE IF LPAR compare RPAR LBRA lines RBRA elsechain
              | ELSE LBRA lines RBRA
              |
              ;
vardecls      : vardecl SEMI vardecls
              |
              ;
vardecl       : TYPE ID
              | TYPE ID ASSIGN expr
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
paramlistcall : ID COMMA paramlistcall
              | ID
              ;
expr          : expr PLUS term
              | expr MINUS term
              | compare COLON expr QUEST expr
              | term
              ;
term          : term TIMES factor
              | term DIV factor
              | factor
              ;
factor        : ID
              | VAL
              | LPAR expr RPAR
              ;
compare       : expr COP expr
              ;
%%

int yyerror(char *s){
    printf("%s", s);
    return 0;
}