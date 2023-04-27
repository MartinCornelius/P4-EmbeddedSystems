%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdint.h>

    int optimize = 0;

    FILE *file;

    #include "include/symbol_types.h"
    #include "include/symbol_table.h"
    #include "include/ast.h"
    #include "include/const_folding.h"
    #include "include/code_gen.h"

    extern FILE *yyin; 

    int yylex();
    int yyerror(char *s);

    struct ast *root;
    struct HashTables* symTable;
%}

%union{ int val; float valf; int type; char* id; char str[500]; char* string; struct ast *node; }

%start prog

%token<val> VAL
%token<valf> VALF
%token<string> COPLE COPGE COPEQ COPNEQ COPL COPG LOGOR LOGAND NOT
%token<id> ID
%token<type> TYPE
%token<string> STRING

%token ROOT DEFINE SETUP MAIN FUNC LARROW
        RARROW LBRA RBRA RPAR LPAR
        PLUS MINUS TIMES DIV SEMI 
        COMMA PRINT 
%token LINES LINE CONTROL FUNCS PARAM PARAMS FUNCCALL EMPTY TERM FACTOR IFELSECHAIN ELSECHAIN DECL /* Extra */
%token ASSIGN WHILE IF ELSEIF ELSE

%type<node> compare comparelist boolexpr funcs func vardecl
%type<node> term factor expr setup mainloop funccall paramincall paramoutcall
%type<node> paramoutdecl paramindecl lines line control elsechain 

%%
prog          : setup mainloop funcs
                { 
                    root = allocASTRootNode(ROOT, $1, $2, $3);
                    printf("\n=========== HASHTABLE ===========\n");
                    symTable = fetchSymbolTable();
                    printTables(symTable);
                    printf("\n=========== AST ===========\n");
                    printAST(root, 0);
                    if (optimize)
                    {
                        printf("\n\n=========== OPTIMIZATIONS ===========\n");
                        constantFolding(root);

                        printf("\n\n=========== OPTIMIZED AST ===========\n");
                        printAST(root, 0);
                    }
                    printf("\n\n=========== LLVM CODE GEN ===========\n");
                    generateFile(root);
                    printf("Done generating file\n");
                    freeAST(root);
                    printf("Done.\n");
                }
              ;
setup         : SETUP LBRA lines RBRA   { changeScope("setup"); $$ = allocAST(SETUP, $3, NULL); }
              ;
mainloop      : MAIN LBRA lines RBRA    {  changeScope("mainloop"); $$ = allocAST(MAIN, $3, NULL); }
              ;
funcs         : func funcs          { $$ = allocAST(FUNCS, $1, $2); }
              |                     { changeScope("func"); $$ = allocAST(FUNCS, NULL, NULL); }                                     
              ;
func          : FUNC ID LPAR paramindecl RPAR LBRA lines RBRA                     
                { 
                    changeScope("func");
                    $$ = allocASTFuncNode(FUNC, allocASTLeafStr(ID, $2), $4, $7); 
                }
              | FUNC ID LPAR paramindecl RARROW paramoutdecl RPAR LBRA lines RBRA 
                { 
                    changeScope("func");
                    $$ = allocASTFuncNode(FUNC, allocASTLeafStr(ID, $2), allocAST(PARAMS, $4, $6), $9); 
                }
              ;
paramoutdecl  : TYPE ID COMMA paramoutdecl         { createSymbol($2, $1); $$ = allocAST(PARAMS, allocASTLeafStr(ID, $2), $4); }                       
              | TYPE ID                            { createSymbol($2, $1); $$ = allocASTLeafStr(ID, $2); }                       
              |                     { $$ = allocAST(EMPTY, NULL, NULL); }                                      
              ;
paramindecl   : TYPE ID COMMA paramindecl          { createSymbol($2, $1); $$ = allocAST(PARAMS, allocASTLeafStr(ID, $2), $4); }                       
              | TYPE ID                            { createSymbol($2, $1); $$ = allocAST(PARAMS, allocASTLeafStr(ID, $2), NULL); }
              |                     { $$ = allocAST(EMPTY, NULL, NULL); }                                      
              ;
lines         : line SEMI lines     { $$ = allocAST(LINES, $1, $3); }
              | control lines       { $$ = allocAST(LINES, $1, $2); }
              |                     { $$ = allocAST(EMPTY, NULL, NULL); }
              ;
line          : ID ASSIGN expr      { $$ = allocAST(ASSIGN, allocASTLeafStr(ID, $1), $3); }
              | ID LARROW expr      { $$ = allocAST(LARROW, allocASTLeafStr(ID, $1), $3); }                                      
              | funccall                              { ; }                    
              | PRINT LPAR ID RPAR                    { $$ = allocAST(PRINT, allocASTLeafStr(ID, $3), NULL); }
              | vardecl                               { $$ = $1; }                    
              ;
control       : WHILE LPAR comparelist RPAR LBRA lines RBRA               { $$ = allocAST(WHILE, $3, $6); }
              | IF LPAR comparelist RPAR LBRA lines RBRA                  { $$ = allocASTIfNode(IF, $3, $6, NULL); } 
              | IF LPAR comparelist RPAR LBRA lines RBRA elsechain        { $$ = allocASTIfNode(IF, $3, $6, $8); }
              ;
elsechain     : ELSE control                                              { $$ = $2; }
              | ELSE LBRA lines RBRA                                      { $$ = $3; }
              ;
vardecl       : TYPE ID
              { createSymbol($2, $1); $$ = allocAST(DECL,
              allocASTLeafStr(ID, $2), NULL); }
              | TYPE ID ASSIGN expr                             
              { createSymbol($2, $1); $$ = allocAST(ASSIGN,
              allocASTLeafStr(ID, $2), $4); }
              | TYPE ID ASSIGN STRING                           { ; }          
              ;
funccall      : ID LPAR paramincall RPAR                        { $$ = allocAST(FUNCCALL, allocASTLeafStr(ID, $1), $3); }          
              | ID LPAR paramincall RARROW paramoutcall RPAR    { $$ = allocAST(FUNCCALL, allocASTLeafStr(ID, $1), allocAST(PARAMS, $3, $5)); }          
              ; 
paramoutcall  : ID COMMA paramoutcall   { $$ = allocAST(PARAMS, allocASTLeafStr(ID, $1), $3); }                                  
              | ID                      { $$ = allocAST(PARAMS, allocASTLeafStr(ID, $1), NULL); }
              ;
paramincall   : ID COMMA paramincall    { $$ = allocAST(PARAMS, allocASTLeafStr(ID, $1), $3); }
              | expr COMMA paramincall  { $$ = allocAST(PARAMS, $1, $3); }                                  
              | ID                  { $$ = allocAST(PARAMS, allocASTLeafStr(ID, $1), NULL); }                                      
              | expr                { $$ = allocAST(PARAMS, $1, NULL); }                                      
              |                     { $$ = allocAST(EMPTY, NULL, NULL); }                                     
              ;
expr          : expr PLUS term      { $$ = allocAST(PLUS, $1, $3); }                                      
              | expr MINUS term     { $$ = allocAST(MINUS, $1, $3); }                                      
              | term                { $$ = $1; }
              ;
term          : term TIMES factor   { $$ = allocAST(TIMES, $1, $3); }                                      
              | term DIV factor     { $$ = allocAST(DIV, $1, $3); }                                      
              | factor              { $$ = $1; }                                      
              ;
factor        : ID                  { $$ = allocASTLeafStr(ID, $1); }                                      
              | VAL                 { $$ = allocASTLeafInt(VAL, $1); }                                      
              | VALF                { $$ = allocASTLeafFloat(VALF, $1); }                                      
              | LPAR expr RPAR      { $$ = $2; }                                      
              ;
comparelist   : compare LOGOR comparelist  { $$ = allocAST(LOGOR, $1, $3); }
              | compare LOGAND comparelist { $$ = allocAST(LOGAND, $1, $3); }                                  
              | compare                    { $$ = $1; }                               
              ;
compare       : boolexpr COPLE compare          { $$ = allocAST(COPLE, $1, $3);     }                          
              | NOT boolexpr COPLE compare      { ; }
              | boolexpr COPGE compare          { $$ = allocAST(COPGE, $1, $3);     }                               
              | NOT boolexpr COPGE compare      { ; }
              | boolexpr COPEQ compare          { $$ = allocAST(COPEQ, $1, $3);     }                               
              | NOT boolexpr COPEQ compare      { ; }
              | boolexpr COPNEQ compare         { $$ = allocAST(COPNEQ, $1, $3);    }                               
              | NOT boolexpr COPNEQ compare     { ; }
              | boolexpr COPL compare           { $$ = allocAST(COPL, $1, $3);      }                               
              | NOT boolexpr COPL compare       { ; }
              | boolexpr COPG compare           { $$ = allocAST(COPG, $1, $3);      }                               
              | NOT boolexpr COPG compare       { ; }                             
              | boolexpr                        { $$ = $1; }                               
              | NOT boolexpr                    { ; }                          
              ;
boolexpr      : LPAR comparelist RPAR           { $$ = $2; }
              | ID                 { $$ = allocASTLeafStr(ID, $1); } 
              | VAL                { $$ = allocASTLeafInt(VAL, $1); }
              ;
%%

void main(int argc, char **argv)
{
    // TODO Determine size at some point
    createMainTable(1000);
    changeScope("globals");

    file = fopen("output/example_program.ll", "w");
    if (argc > 1)
      if (!(yyin = fopen(argv[1], "r")))
        perror("Error loading file\n");
    /* if (argc > 2)
        sprintf(outputFile, "output/%s", argv[2]); */

    if (argc > 2)
        if (strcmp(argv[2], "1") == 0) {
            optimize = 1;
        }

    yyparse();
    fclose(file);
}

int yyerror(char *s){
    printf("The error: %s", s);
    return 0;
}
