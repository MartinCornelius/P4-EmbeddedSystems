%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdint.h>

    // String that will become the C file
    char temp[500];
    char programString[3000];

    #include "include/symbol_types.h"
    #include "include/ast.h"
    #include "include/code_gen.h"

    extern FILE *yyin; 
    char outputFile[512] = "output/output.c";

    int yylex();
    int yyerror(char *s);

    // Start of linked list
    Symbol_Struct* handle;
    // Last element in list
    void *listHead;
%}

%union{ int val; float valf; int type; char* id; char str[500]; char* string; struct ast *node; }

%start prog

%token<val> VAL
%token<valf> VALF
%token<string> COP LOGOP NOT
%token<id> ID
%token<type> TYPE
%token<string> STRING

%token DEFINE SETUP MAIN FUNC LARROW
        RARROW LBRA RBRA RPAR LPAR
        PLUS MINUS TIMES DIV SEMI 
        COMMA PRINT
%token ASSIGN WHILE IF ELSE


%type<node> compare comparelist boolexpr funcs func vardecl
%type<node> term factor expr defines define setup mainloop funccall paramincall paramoutcall
%type<node> paramoutdecl paramindecl lines line control elsechain

%%
prog          : defines funcs setup mainloop    { printAST($4); printf("\nresult: %d\n", evalAST($4)); printf("Done.\n"); }                          
              ;
defines       : define defines                                            
              |                                                           
              ;
define        : DEFINE ID expr                                            
              ;
setup         : SETUP LBRA lines RBRA                                     
              ;
mainloop      : MAIN LBRA lines RBRA    { $$ = $3; }                                  
              ;
funcs         : func funcs                                                
              |                                                           
              ;
func          : FUNC ID LPAR paramindecl RPAR LBRA lines RBRA                 
              | FUNC ID LPAR paramindecl RARROW paramoutdecl RPAR LBRA lines RBRA 
              ;
paramoutdecl  : TYPE ID COMMA paramoutdecl                                
              | TYPE ID                                                   
              |                                                           
              ;
paramindecl   : TYPE ID COMMA paramindecl                                 
              | TYPE ID                                                   
              |                                                           
              ;
lines         : line SEMI lines     { $$ = $1; }                                      
              | control lines                                             
              |                                                           
              ;
line          : ID ASSIGN expr      { $$ = $3; }                                      
              | ID LARROW expr                                            
              | funccall                                                  
              | PRINT LPAR STRING RPAR                                    
              | vardecl                                                   
              |                                                           
              ;
control       : WHILE LPAR comparelist RPAR LBRA lines RBRA               
              | IF LPAR comparelist RPAR LBRA lines RBRA elsechain        
              ;
elsechain     : ELSE IF LPAR comparelist RPAR LBRA lines RBRA elsechain   
              | ELSE LBRA lines RBRA                                      
              |                                                           
              ;
vardecl       : TYPE ID                                                   
              | TYPE ID ASSIGN expr                                      
              | TYPE ID ASSIGN STRING                                     
              ;
funccall      : ID LPAR paramincall RPAR                                  
              | ID LPAR paramincall RARROW paramoutcall RPAR              
              ;
paramoutcall  : ID COMMA paramoutcall                                     
              | ID                                                        
              ;
paramincall   : ID COMMA paramincall                                      
              | expr COMMA paramincall                                    
              | ID                                                        
              | expr                                                      
              |                                                           
              ;
expr          : expr PLUS term      { $$ = allocAST(PLUS, $1, $3); }                                      
              | expr MINUS term                                           
              | term                { $$ = $1; }                                      
              ;
term          : term TIMES factor                                         
              | term DIV factor                                           
              | factor              { $$ = $1; }                                      
              ;
factor        : ID                                                        
              | VAL                 { $$ = allocASTLeaf($1); }                                      
              | VALF                                                      
              | LPAR expr RPAR                                            
              ;
comparelist   : compare LOGOP comparelist                                 
              | compare                                                   
              ;
compare       : boolexpr COP compare                                      
              | NOT boolexpr COP compare                                  
              | boolexpr                                                  
              | NOT boolexpr                                              
              ;
boolexpr      : LPAR comparelist RPAR                                     
              | ID                  
              | VAL        
              ;
%%

void main(int argc, char **argv)
{
    if (argc > 1)
      if (!(yyin = fopen(argv[1], "r")))
        perror("Error loading file\n");
    if (argc > 2)
        sprintf(outputFile, "output/%s", argv[2]);

    yyparse();
}

int yyerror(char *s){
    printf("The error: %s", s);
    return 0;
}
