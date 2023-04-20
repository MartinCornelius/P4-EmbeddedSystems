
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     VAL = 258,
     VALF = 259,
     COPLE = 260,
     COPGE = 261,
     COPEQ = 262,
     COPNEQ = 263,
     COPL = 264,
     COPG = 265,
     LOGOR = 266,
     LOGAND = 267,
     NOT = 268,
     ID = 269,
     TYPE = 270,
     STRING = 271,
     ROOT = 272,
     DEFINE = 273,
     SETUP = 274,
     MAIN = 275,
     FUNC = 276,
     LARROW = 277,
     RARROW = 278,
     LBRA = 279,
     RBRA = 280,
     RPAR = 281,
     LPAR = 282,
     PLUS = 283,
     MINUS = 284,
     TIMES = 285,
     DIV = 286,
     SEMI = 287,
     COMMA = 288,
     PRINT = 289,
     LINES = 290,
     LINE = 291,
     CONTROL = 292,
     FUNCS = 293,
     EMPTY = 294,
     TERM = 295,
     FACTOR = 296,
     IFELSECHAIN = 297,
     ELSECHAIN = 298,
     DECL = 299,
     ASSIGN = 300,
     WHILE = 301,
     IF = 302,
     ELSEIF = 303,
     ELSE = 304
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 39 "src/parser.y"
 int val; float valf; int type; char* id; char str[500]; char* string; struct ast *node; 


/* Line 1676 of yacc.c  */
#line 105 "src/parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


