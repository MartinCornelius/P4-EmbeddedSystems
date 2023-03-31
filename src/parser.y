%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdint.h>

    // String that will become the C file
    char temp[500];
    char programString[3000];

    #include "include/symbol_table.h"
    #include "include/symbol_types.h"
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

%union{ int val; float valf; int type; char* id; char str[500]; char* string; }

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


%type<str> compare comparelist boolexpr funcs func vardecl
%type<str> term factor expr defines define setup mainloop funccall paramincall paramoutcall
%type<str> paramoutdecl paramindecl lines line control elsechain

%%
prog          : defines funcs setup mainloop                                { emit("}", programString); printToFile(outputFile, programString); }
              ;
defines       : define defines                                              { sprintf(temp, "%s\n%s", $1, $2); strcpy($$, temp); }
              |                                                             { strcpy($$, ""); }
              ;
define        : DEFINE ID expr                                              { sprintf(temp, "#define %s %s\n", $2, $3); emit(temp, programString); }
              ;
setup         : SETUP LBRA lines RBRA                                       { sprintf(temp, "%s\nif(1){\n", $3); emit(temp, programString); }
              ;
mainloop      : MAIN LBRA lines RBRA                                        { sprintf(temp, "%s}\n", $3); emit(temp, programString); }
              ;
funcs         : func funcs                                                  { strcpy($$, ""); }
              |                                                             { emit("void main(){\n", programString); }
              ;
func          : FUNC ID LPAR paramindecl RPAR LBRA lines RBRA                       { sprintf(temp, "void %s(%s){\n%s}\n", $2, $4, $7); emit(temp, programString); }
              | FUNC ID LPAR paramindecl RARROW paramoutdecl RPAR LBRA lines RBRA   { sprintf(temp, "void %s(%s,%s){\n%s}\n", $2, $4, $6, $9); emit(temp, programString); }
              ;
paramoutdecl  : TYPE ID COMMA paramoutdecl                                  { char type[20]; typeToString(type, $1); sprintf(temp, "%s *%s, %s", type, $2, $4); strcpy($$, temp); }
              | TYPE ID                                                     { char type[20]; typeToString(type, $1); sprintf(temp, "%s *%s", type, $2); strcpy($$, temp); }
              |                                                             { strcpy($$, ""); }
              ;
paramindecl   : TYPE ID COMMA paramindecl                                   { char type[20]; typeToString(type, $1); sprintf(temp, "%s %s, %s", type, $2, $4); strcpy($$, temp); }  
              | TYPE ID                                                     { char type[20]; typeToString(type, $1); sprintf(temp, "%s %s", type, $2); strcpy($$, temp); }         
              |                                                             { strcpy($$, ""); }
              ;
lines         : line SEMI lines                                             { sprintf(temp, "%s%s", $1, $3); strcpy($$, temp); }
              | control lines                                               { sprintf(temp, "%s%s", $1, $2); strcpy($$, temp); }
              |                                                             { strcpy($$, ""); }
              ;
line          : ID ASSIGN expr                                              { sprintf(temp, "%s = %s;\n", $1, $3); strcpy($$, temp); }
              | ID LARROW expr                                              { sprintf(temp, "*%s = %s;\n", $1, $3); strcpy($$, temp); }
              | funccall                                                    { strcpy($$, $1); }
              | PRINT LPAR STRING RPAR                                      { sprintf(temp, "printf(%s);\n", $3); strcpy($$, temp); }
              | vardecl                                                     { strcpy($$, $1); }
              |                                                             { strcpy($$, ""); }
              ;
control       : WHILE LPAR comparelist RPAR LBRA lines RBRA                 { sprintf(temp, "while(%s){\n%s\n}\n", $3, $6); strcpy($$, temp); }
              | IF LPAR comparelist RPAR LBRA lines RBRA elsechain          { sprintf(temp, "if(%s){\n%s\n}%s\n", $3, $6, $8); strcpy($$, temp); }
              ;
elsechain     : ELSE IF LPAR comparelist RPAR LBRA lines RBRA elsechain     { sprintf(temp, "else if(%s){\n%s\n}%s", $4, $7, $9); strcpy($$, temp); }
              | ELSE LBRA lines RBRA                                        { sprintf(temp, "else{\n%s\n}", $3); strcpy($$, temp); }
              |                                                             { strcpy($$, ""); }
              ;
vardecl       : TYPE ID                                                     { char type[20]; typeToString(type, $1); printf("vardecl 94 %s %s = %s;\n", type, $2); sprintf(temp, "%s %s;\n", type, $2); strcpy($$, temp); }
              | TYPE ID ASSIGN expr                                         { char type[20]; createSymbol($1, $2); typeToString(type, $1);  printf("vardecl 95: %s %s = %s;\n", type, $2, $4); sprintf(temp, "%s %s = %s;\n", type, $2, $4); strcpy($$, temp); }
              | TYPE ID ASSIGN STRING                                       { char type[20]; typeToString(type, $1); sprintf(temp, "%s %s = %s;\n", type, $2, $4); strcpy($$, temp); }
              ;
funccall      : ID LPAR paramincall RPAR                                    { sprintf(temp, "%s(%s);\n", $1, $3); strcpy($$, temp); }
              | ID LPAR paramincall RARROW paramoutcall RPAR                { sprintf(temp, "%s(%s, %s);\n", $1, $3, $5); strcpy($$, temp); }
              ;
paramoutcall  : ID COMMA paramoutcall                                       { sprintf(temp, "&%s, %s", $1, $3); strcpy($$, temp); }
              | ID                                                          { sprintf(temp, "&%s", $1); strcpy($$, temp); }
              ;
paramincall   : ID COMMA paramincall                                        { sprintf(temp, "%s, %s", $1, $3); strcpy($$, temp); }
              | expr COMMA paramincall                                      { sprintf(temp, "%s, %s", $1, $3); strcpy($$, temp); }
              | ID                                                          { strcpy($$, $1); }
              | expr                                                        { strcpy($$, $1); }
              |                                                             { strcpy($$, ""); }
              ;
expr          : expr PLUS term                                              { sprintf(temp, "%s + %s", $1, $3); strcpy($$, temp); }                        
              | expr MINUS term                                             { printf("Expr minus term: %s - %s", $1, $3); sprintf(temp, "%s - %s", $1, $3); strcpy($$, temp); }
              | term                                                        { strcpy($$, $1); }
              ;
term          : term TIMES factor                                           { strcpy($$, temp); }
              | term DIV factor                                             { sprintf(temp, "%s / %s", $1, $3); strcpy($$, temp); }
              | factor                                                      { strcpy($$, $1);}
              ;
factor        : ID                                                          { strcpy($$, $1); }
              | VAL                                                         { sprintf(temp, "%d", $1); strcpy($$, temp);  }
              | VALF                                                        { sprintf(temp, "%f", $1); strcpy($$, temp);  }
              | LPAR expr RPAR                                              { sprintf(temp, "(%s)", $2); strcpy($$, temp); }
              ;
comparelist   : compare LOGOP comparelist                                   { sprintf(temp, "%s %s %s", $1, $2, $3); strcpy($$, temp); }
              | compare                                                     { strcpy($$, $1); }
              ;
compare       : boolexpr COP compare                                        { sprintf(temp, "%s %s %s", $1, $2, $3); strcpy($$, temp); }
              | NOT boolexpr COP compare                                    { sprintf(temp, "!%s %s %s", $2, $3, $4); strcpy($$, temp); }
              | boolexpr                                                    { strcpy($$, $1); }
              | NOT boolexpr                                                { sprintf(temp, "!%s", $2); strcpy($$, temp); }
              ;
boolexpr      : LPAR comparelist RPAR                                       { sprintf(temp, "(%s)", $2); strcpy($$, temp); }
              | ID                                                          { strcpy($$, $1); }
              | VAL                                                         { sprintf(temp, "%d", $1); strcpy($$, temp); }
              ;
%%

void main(int argc, char **argv)
{
    emit("#include <stdio.h>\n#include <stdint.h>\n", programString);
    // handle->next = NULL;
    // listHead = (struct Symbol *)handle;
    HashTable *hTable = createTable(100);

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
