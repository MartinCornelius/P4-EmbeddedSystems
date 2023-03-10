%option noyywrap

%{
#include <stdio.h>
#include "Parser.tab.h"

%}

%x COMMENT

alpha                   [a-zA-Z]
digit                   [0-9]

%%
"//"[ \t].*             { ; } /* SINGLE LINE COMMENT */
"/*"                    { BEGIN(COMMENT); } /* MULTI LINE COMMENT */
<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>.              {  }

"->"                    { return LARROW; }
"<-"                    { return RARROW; }

"("                     { return LBRA; }
")"                     { return RBRA; }

"{"                     { return RPAR; }
"}"                     { return LPAR; }

"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return TIMES; }
"/"                     { return DIV; }

":"                     { return COLON; }
"?"                     { return QUEST; }

";"                     { return SEMI; }
","                     { return COMMA; }

"import"                { return IMPORT; }
"<"{alpha}+".h>"        { return HEADERF; }

"define"                { return DEFINE; }
"setup()"               { return SETUP; }
"mainloop()"            { return MAIN; }
"func"                  { return FUNC; }

"bool" |
"int8" |
"int16" |
"uint8" |
"uint16" |
"float8" |
"float16" |
"char"                  { return TYPE; }

"="                     { return ASSIGN; }
"while"                 { return WHILE; }
"if"                    { return IF; }

"else"                  { return ELSE; }
"<" |
"<=" |
">" |
">=" |
"==" |
"!="                    { return COP; }

[0] |
[1-9]{digit}* |
[1-9]{digit}*\.{digit}+ |
[0]\.{digit}+           { yylval.val = atof(yytext); return VAL; }
{alpha}+                { yylval.id = yytext ; return ID; }
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