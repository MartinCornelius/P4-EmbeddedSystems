%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdint.h>
    #include "include/symbolstructs.h"

    extern FILE *yyin; 

    enum types { symbol_enum = -1, input_enum, output_enum, int8_enum, int16_enum, uint8_enum, 
                 uint16_enum, float8_enum, float16_enum, bool_enum, char_enum, flexint_enum };

    int yylex();
    int yyerror(char *s);
    Symbol_Struct* findSymbol(char* name);
    void createToken(int type, char* name);
    void changeTokenVal(char* name, char* val);

    int cOp(int x, char* op, int y);
    int logCop(int x, char* logop, int y);
    // Emitter
    void emit(const char* input);

    void typeToString(char* input, int type); 

    // For debugging skal fjernes senere
    void printTable();
    void printToFile();

    // Start of linked list
    Symbol_Struct handle;
    // Last element in list
    void *listHead;

    // String that will become the C file
    char programString[3000];
    char temp[500];
%}

%union{ int val; int type; char* id; char str[500]; int boolean; char* string; }

%start prog

%token<val> VAL
%token<string> COP LOGOP
%token<id> ID
%token<type> TYPE
%token<boolean> BOOLVAL
%token<str> STRING


%token DEFINE SETUP MAIN FUNC LARROW
        RARROW LBRA RBRA RPAR LPAR
        PLUS MINUS TIMES DIV SEMI 
        COMMA PRINT
%token ASSIGN WHILE IF ELSE


%type<str> compare comparelist boolexpr funcs func vardecl
%type<str> term factor expr defines define setup mainloop funccall paramincall paramoutcall
%type<str> paramoutdecl paramindecl lines line control elsechain

%%
prog          : defines funcs setup mainloop                                { emit("}"); printToFile(); }
              ;
defines       : define defines                                              { sprintf(temp, "%s\n%s", $1, $2); strcpy($$, temp); }
              |                                                             { strcpy($$, ""); }
              ;
define        : DEFINE ID expr                                              { sprintf(temp, "#define %s %s\n", $2, $3); emit(temp); }
              ;
setup         : SETUP LBRA lines RBRA                                       { sprintf(temp, "%s\nwhile(1){\n", $3); emit(temp); }
              ;
mainloop      : MAIN LBRA lines RBRA                                        { sprintf(temp, "%s}\n", $3); printf("%s\n", temp); emit(temp); }
              ;
funcs         : func funcs                                                  { strcpy($$, ""); }
              |                                                             { emit("void main(){\n"); }
              ;
func          : FUNC ID LPAR paramindecl RPAR LBRA lines RBRA                       { sprintf(temp, "void %s(%s){\n%s}\n", $2, $4, $7); emit(temp); }
              | FUNC ID LPAR paramindecl RARROW paramoutdecl RPAR LBRA lines RBRA   { sprintf(temp, "void %s(%s,%s){\n%s}\n", $2, $4, $6, $9); emit(temp); }
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
line          : ID ASSIGN expr                                              { /*changeTokenVal($1, $3);*/ sprintf(temp, "%s = %s;\n", $1, $3); strcpy($$, temp); }
              | ID LARROW expr                                              { /*changeTokenVal($1, $3);*/ sprintf(temp, "*%s = %s;\n", $1, $3); strcpy($$, temp); }
              | funccall                                                    { strcpy($$, $1); }
              | PRINT LPAR STRING RPAR                                      { sprintf(temp, "printf(%s;\n", $3); strcpy($$, temp); }
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
vardecl       : TYPE ID                                                     { createToken($1, $2);                         char type[20]; typeToString(type, $1); sprintf(temp, "%s %s;\n", type, $2); strcpy($$, temp); }
              | TYPE ID ASSIGN expr                                         { createToken($1, $2); changeTokenVal($2, $4); char type[20]; typeToString(type, $1); sprintf(temp, "%s %s = %s;\n", type, $2, $4); strcpy($$, temp); }
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
              | expr MINUS term                                             { sprintf(temp, "%s - %s", $1, $3); strcpy($$, temp); }
              | term                                                        { strcpy($$, $1); }
              ;
term          : term TIMES factor                                           { sprintf(temp, "%s * %s", $1, $3); strcpy($$, temp); }
              | term DIV factor                                             { sprintf(temp, "%s / %s", $1, $3); strcpy($$, temp); }
              | factor                                                      { strcpy($$, $1);}
              ;
factor        : ID                                                          { strcpy($$, $1); }
              | VAL                                                         { sprintf(temp, "%d", $1); strcpy($$, temp);  }
              | LPAR expr RPAR                                              { sprintf(temp, "(%s)", $2); strcpy($$, temp); }
              ;
comparelist   : compare LOGOP comparelist                                   { sprintf(temp, "%s %s %s", $1, $2, $3); strcpy($$, temp); }
              | compare                                                     { strcpy($$, $1); }
              ;
compare       : boolexpr COP compare                                        { sprintf(temp, "%s %s %s", $1, $2, $3); strcpy($$, temp); }
              | boolexpr                                                    { strcpy($$, $1); }
              ;
boolexpr      : ID                                                          { strcpy($$, $1); }
              | VAL                                                         { sprintf(temp, "%d", $1); strcpy($$, temp); }
              ;
%%

void main(int argc, char **argv)
{
    emit("#include <stdio.h>\n#include <stdint.h>\n");
    handle.next = NULL;
    listHead = (struct Symbol *)&handle;
    if (argc > 1)
      if (!(yyin = fopen(argv[1], "r")))
        perror("Error loading file\n");
    yyparse();
}

void printToFile()
{
    FILE *fp = fopen("program.c", "w");
    fprintf(fp, "%s", programString);
    fclose(fp);
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

void changeTokenVal(char* name, char* sval)
{
    int val = atoi(sval);
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

void emit(const char* input)
{
    printf("\n%s\n", input);
    strcat(programString, input);
}

void typeToString(char* input, int type)
{
    
    switch(type)
    {
        case 2:
            strcpy(input, "int8_t");
            break;
        case 6:
            strcpy(input, "float");
            break;
        default:
            printf("invalid type given, defaulting to flexint\n");
            strcpy(input, "flexint");
    }
}

int yyerror(char *s){
    printf("The error: %s", s);
    printTable();
    /* printToFile(); */
    return 0;
}
