%option noyywrap
%{

#include "parser.tab.h"
#include <string.h>

%}

%x COMMENT

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

"+"                       { return PLUS;  }
"-"                       { return MINUS; }
"*"                       { return TIMES; }
"/"                       { return DIV;   }

";"                       { return SEMI;  }
","                       { return COMMA; }

"define"                  { return DEFINE; }
"setup()"                 { return SETUP;  }
"mainloop()"              { return MAIN;   }
"func"                    { return FUNC;   }
"print"                   { return PRINT;  }

\"[A-Za-z%\\/ ]*\"[\,a-zA-Z ]*     { yylval.string = strdup(yytext); return STRING; }

"input"                   { yylval.type = 0; return TYPE; }
"output"                  { yylval.type = 1; return TYPE; }
"int8"                    { yylval.type = 2; return TYPE; }
"int16"                   { yylval.type = 3; return TYPE; }
"uint8"                   { yylval.type = 4; return TYPE; }
"uint16"                  { yylval.type = 5; return TYPE; }
"float8"                  { yylval.type = 6; return TYPE; }
"float16"                 { yylval.type = 7; return TYPE; }
"bool"                    { yylval.type = 8; return TYPE; }
"char"                    { yylval.type = 9; return TYPE; }
"int"                     { yylval.type = 10; return TYPE;}

"="                       { return ASSIGN; }
"while"                   { return WHILE;  }
"if"                      { return IF;     }

"else"                    { return ELSE;   }

"<=" |
">=" |
"==" |
"!=" |
"<"  |
">"                       { yylval.string = strdup(yytext); return COP;   }

"||" |
"&&"                      { yylval.string = strdup(yytext); return LOGOP; }

"0" |
[1-9]{digit}*             { yylval.val = atoi(yytext); return VAL; }
[a-zA-Z_][0-9a-zA-Z_]*    { yylval.id = strdup(yytext); return ID; }
[ \t\n]                   { /*Ignore whitespace*/; }
.                         { printf("%s", yytext); }
%%

