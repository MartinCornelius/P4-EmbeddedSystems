%option noyywrap
%{

#include "parser.tab.h"
#include <string.h>

#include "include/symbol_types.h"

%}

%x COMMENT

alpha                   [a-zA-Z]
digit                   [0-9]

%%
"//"[ \t].*               { ; } /* SINGLE LINE COMMENT */
"/*"                      { BEGIN(COMMENT); } /* MULTI LINE COMMENT */
<COMMENT>"*/"             { BEGIN(INITIAL); }
<COMMENT>.                {  }


"define"                  { return DEFINE; }
"setup()"                 { return SETUP;  }
"mainloop()"              { return MAIN;   }
"func"                    { return FUNC;   }
"print"                   { return PRINT;  }

\'{alpha}\'                        { yylval.string = strdup(yytext); return STRING; }
\"[A-Za-z%\\/ ]*\"[\,a-zA-Z ]*     { yylval.string = strdup(yytext); return STRING; }

"input"                   { yylval.type = input_enum; return TYPE; }
"output"                  { yylval.type = output_enum; return TYPE; }
"int8"                    { yylval.type = int8_enum; return TYPE; }
"int16"                   { yylval.type = int16_enum; return TYPE; }
"int32"                   { yylval.type = int32_enum; return TYPE; }
"uint8"                   { yylval.type = uint8_enum; return TYPE; }
"uint16"                  { yylval.type = uint16_enum; return TYPE; }
"uint32"                  { yylval.type = uint32_enum; return TYPE; }
"float"                   { yylval.type = float_enum; return TYPE; }
"bool"                    { yylval.type = bool_enum; return TYPE; }
"char"                    { yylval.type = char_enum; return TYPE; }
"int"                     { yylval.type = flexint_enum; return TYPE;}

"="                       { return ASSIGN; }
"while"                   { return WHILE;  }
"if"                      { return IF;     }

"else"                    { return ELSE;   }

"<="                      { yylval.string = strdup(yytext); return COPLE;  }
">="                      { yylval.string = strdup(yytext); return COPGE;  }
"=="                      { yylval.string = strdup(yytext); return COPEQ;  }
"!="                      { yylval.string = strdup(yytext); return COPNEQ; }
"<"                       { yylval.string = strdup(yytext); return COPL;   }
">"                       { yylval.string = strdup(yytext); return COPG;   }

"!"                       { return NOT;   }

"||"                      { yylval.string = strdup(yytext); return LOGOR; }
"&&"                      { yylval.string = strdup(yytext); return LOGAND; }

"-0x"[0-9a-fA-F]+ |
"0x"[0-9a-fA-F]+          { yylval.string = strdup(yytext); return STRING; }

"0" |
"-"[1-9]{digit}* |
[1-9]{digit}*             { yylval.val = atoi(yytext); return VAL; }

"-0."{digit}+ |
"-"[1-9]{digit}*"."{digit}+ |
"0."{digit}+ |
[1-9]{digit}*"."{digit}+  { yylval.valf = atof(yytext); return VALF; }

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

[a-zA-Z_][0-9a-zA-Z_]*    { yylval.id = strdup(yytext); return ID; }
[ \t\n]                   { /*Ignore whitespace*/; }
.                         { printf("%s", yytext); }
%%

