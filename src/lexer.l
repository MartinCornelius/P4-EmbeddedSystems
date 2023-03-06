%option noyywrap

%{
#include <stdio.h>
%}

%x COMMENT

alpha                   [a-zA-Z]
digit                   [0-9]

%%
"//"[ \t].*             { ; } /* SINGLE LINE COMMENT */
"/*"                    { BEGIN(COMMENT); } /* MULTI LINE COMMENT */
<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>.              {  }

{digit}+                { printf("Int: %d\n", atoi(yytext)); }
{alpha}+                { printf("String: %s\n", yytext); }
[ \t\n]                 { ; }
.                       { printf("Not implemented\n"); }
%%

void main(int argc, char **argv)
{
    if (argc > 1)
      if (!(yyin = fopen(argv[1], "r")))
        perror(argv[1]);
    yylex();
}

