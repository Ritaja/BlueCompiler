%option noyywrap

%{
#include "parser.tab.h"
#include <stdlib.h>
%}

%option noyywrap
%option yylineno

%%

"while" return TOKEN_WHILE;
"!="    {yylval.op = _strdup(yytext); return TOKEN_OPERATOR;}
"<="    {yylval.op = _strdup(yytext); return TOKEN_OPERATOR;}
">="    {yylval.op = _strdup(yytext); return TOKEN_OPERATOR;}
"&&"    {yylval.op = _strdup(yytext); return TOKEN_OPERATOR;}
"||"    {yylval.op = _strdup(yytext); return TOKEN_OPERATOR;}
"{"   return TOKEN_BEGIN;
"}"   return TOKEN_END;
"["   return BOX_OPEN;
"]"   return BOX_CLOSE;
"func" return BEGIN_FUNC;
"do"  return TOKEN_DO;
"if"  return TOKEN_IF;
"else" return TOKEN_ELSE;
"vector" return TOKEN_VECTOR;
"vector2d" return TOKEN_VECTOR2d;
"return" return TOKEN_RETURN;
"pow"	return TOKEN_POW;
"factorial" return TOKEN_FACTORIAL;
"acos" return TOKEN_ACOS;
"sqrt" return TOKEN_SQRT;
"rotatez" return TOKEN_ROTATEZ;
"magnitude_squared" return TOKEN_MAGNITUDESQR;
"transform" return TOKEN_TRANSFORM;
"min" return TOKEN_MIN;
"dot" return TOKEN_DOT;
"cross" return TOKEN_CROSS;
","    return TOKEN_COMMA;
"=="    {yylval.op = _strdup(yytext); return TOKEN_OPERATOR;}
[a-zA-Z_][a-zA-Z0-9_]* {yylval.name = _strdup(yytext); return TOKEN_ID;}
[-]?[0-9]*[.]?[0-9]+    {yylval.val = atof(yytext); return TOKEN_NUMBER;}
[()=;]  {return *yytext;}
[*/+-<>%] {yylval.op = _strdup(yytext); return TOKEN_OPERATOR;}
[ \t\n] {/* suppress the output of the whitespaces from the input file to stdout */}
#.* {/* one-line comment */}

%%