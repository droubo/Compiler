/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IF = 258,
    ELSE = 259,
    WHILE = 260,
    FOR = 261,
    FUNCTION = 262,
    RETURN = 263,
    BREAK = 264,
    CONTINUE = 265,
    AND = 266,
    OR = 267,
    NOT = 268,
    local = 269,
    TRUE = 270,
    FALSE = 271,
    NIL = 272,
    ASSIGN = 273,
    PLUS = 274,
    MINUS = 275,
    MULT = 276,
    DIV = 277,
    MOD = 278,
    EQUAL = 279,
    NOT_EQUAL = 280,
    PLUS_PLUS = 281,
    MINUS_MINUS = 282,
    GREATER = 283,
    LESS = 284,
    GREATER_EQUAL = 285,
    LESS_EQUAL = 286,
    REALCONST = 287,
    INTCONST = 288,
    STRING = 289,
    LEFT_BRACE = 290,
    RIGHT_BRACE = 291,
    LEFT_BRACKET = 292,
    RIGHT_BRACKET = 293,
    LEFT_PARENTHESIS = 294,
    RIGHT_PARENTHESIS = 295,
    SEMICOLON = 296,
    COMA = 297,
    COLON = 298,
    DOUBLE_COLON = 299,
    DOT = 300,
    DOUBLE_DOT = 301,
    ID = 302,
    UMINUS = 303
  };
#endif
/* Tokens.  */
#define IF 258
#define ELSE 259
#define WHILE 260
#define FOR 261
#define FUNCTION 262
#define RETURN 263
#define BREAK 264
#define CONTINUE 265
#define AND 266
#define OR 267
#define NOT 268
#define local 269
#define TRUE 270
#define FALSE 271
#define NIL 272
#define ASSIGN 273
#define PLUS 274
#define MINUS 275
#define MULT 276
#define DIV 277
#define MOD 278
#define EQUAL 279
#define NOT_EQUAL 280
#define PLUS_PLUS 281
#define MINUS_MINUS 282
#define GREATER 283
#define LESS 284
#define GREATER_EQUAL 285
#define LESS_EQUAL 286
#define REALCONST 287
#define INTCONST 288
#define STRING 289
#define LEFT_BRACE 290
#define RIGHT_BRACE 291
#define LEFT_BRACKET 292
#define RIGHT_BRACKET 293
#define LEFT_PARENTHESIS 294
#define RIGHT_PARENTHESIS 295
#define SEMICOLON 296
#define COMA 297
#define COLON 298
#define DOUBLE_COLON 299
#define DOT 300
#define DOUBLE_DOT 301
#define ID 302
#define UMINUS 303

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 99 "parser.y" /* yacc.c:1909  */
int integer; char* id; double real;

#line 153 "parser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
