
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
     NAME = 258,
     FILENAME = 259,
     ERROR_STR = 260,
     STRING = 261,
     INT_NUM = 262,
     FLOAT_NUM = 263,
     CREATE = 264,
     DROP = 265,
     SELECT = 266,
     INSERT = 267,
     DELETE = 268,
     QUIT = 269,
     EXECFILE = 270,
     TABLE = 271,
     INDEX = 272,
     PRIMARY = 273,
     KEY = 274,
     VALUES = 275,
     FROM = 276,
     WHERE = 277,
     INTO = 278,
     ON = 279,
     UNIQUE = 280,
     AND = 281,
     ENDL = 282,
     CHAR = 283,
     INT = 284,
     FLOAT = 285,
     COMPARISON = 286
   };
#endif
/* Tokens.  */
#define NAME 258
#define FILENAME 259
#define ERROR_STR 260
#define STRING 261
#define INT_NUM 262
#define FLOAT_NUM 263
#define CREATE 264
#define DROP 265
#define SELECT 266
#define INSERT 267
#define DELETE 268
#define QUIT 269
#define EXECFILE 270
#define TABLE 271
#define INDEX 272
#define PRIMARY 273
#define KEY 274
#define VALUES 275
#define FROM 276
#define WHERE 277
#define INTO 278
#define ON 279
#define UNIQUE 280
#define AND 281
#define ENDL 282
#define CHAR 283
#define INT 284
#define FLOAT 285
#define COMPARISON 286




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 19 "minisql.y"

	int intval;
	double floatval;
	char *strval;
	int compare_op;
    char* condition;



/* Line 1676 of yacc.c  */
#line 124 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


