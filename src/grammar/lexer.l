%{
#include <stdio.h>
%}

%%
[0-9]+                  { printf("Int: %d\n", atoi(yytext)); }
[a-zA-Z_]+              { printf("String: %s\n", yytext); }
[ \t\n]                 { printf("fuck you"); }
.                       { printf("Not implemented\n"); }

%%

void main(void)
{
    printf("> ");
    yylex();    
}

int yywrap() { return 1; }