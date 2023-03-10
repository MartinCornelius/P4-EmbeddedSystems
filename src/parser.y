%{

%}



%%
prog          : imports defines funcs setup mainloop
              ;
imports       : import imports
              |
              ;
import        : IMPORT LANG HEADERF RANG
              ;
defines       : define defines
              |
              ;
define        : DEFINE ID expr
              ;
setup         : SETUP LBRA vardecls RBRA
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
return        : ID LARROW Expr SEMI
paramsdecl    : paramlistdecl
              |
              ;
paramlistdecl : TYPE ID COMMA paramlistdecl
              | TYPE ID
              ;
lines         : line SEMI lines
              |
              ;
line          : ID ASSIGN Expr
              | expr
              | funccall
              ;
vardecls      : vardecl SEMI vardecls
              |
              ;
vardecl       : TYPE ID
              | TYPE ID ASSIGN expr
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
              | term
              ;
term          | term TIMES factor
              | term DIV factor
              | factor
              ;
factor        : ID
              | VAL
              | LPAR expr RPAR
              ;
%%
