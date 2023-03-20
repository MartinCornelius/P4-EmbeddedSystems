
%{

#include "parser.tab.h"
#include <string.h>

%}

%x COMMENT

alpha                   [a-zA-Z]
digit                   [0-9]

%%
"//"[ \t].*               { ; } /* SINGLE LINE COMMENT */
"/*"                      { BEGIN(COMMENT); } /* MULTI LINE COMMENT */
<COMMENT>"*/"             { BEGIN(INITIAL); }
<COMMENT>.                {  }

"->"                      { return RARROW; }
"<-"                      { return LARROW; }

"("                       { return LPAR; }
")"                       { return RPAR; }

"{"                       { return LBRA; }
"}"                       { return RBRA; }

"+"                       { return PLUS; }
"-"                       { return MINUS; }
"*"                       { return TIMES; }
"/"                       { return DIV; }

":"                       { return COLON; }
"?"                       { return QUEST; }

";"                       { return SEMI; }
","                       { return COMMA; }

"define"                  { return DEFINE; }
"setup()"                 { return SETUP; }
"mainloop()"              { return MAIN; }
"func"                    { return FUNC; }


"int"                     { yylval.type = yytext; return TYPE; }


"="                       { return ASSIGN; }
"while"                   { return WHILE; }
"if"                      { return IF; }

"else"                    { return ELSE; }

"<" |
"<=" |
">" |
">=" |
"==" |
"!="                      { return COP; }

"||" |
"&&"                      { return LOGOP; }

"true" |
"false"                   { yylval.boolean = yytext[0] == 't'; return BOOLVAL; }

[0] |
[1-9]{digit}*             { yylval.val = atoi(yytext); return VAL; }
[a-zA-Z_][0-9a-zA-Z_]*    { yylval.id = yytext; printf("\n Lex test: %s, %s \n",yytext, yylval.id); return ID; }
[ \t\n]                   { ; }
.                         { printf("Not implemented\n"); }
%%


int yywrap()
{ 
  return 1; 

}

 