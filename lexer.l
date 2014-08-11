%option noyywrap

%{
#include "parser.tab.h"
#include <stdlib.h>
%}

%option noyywrap

%%

"while" return TOKEN_WHILE;
"{"   return TOKEN_BEGIN;
"}"   return TOKEN_END;
"do"  return TOKEN_DO;
"if"  return TOKEN_IF;
"else" return TOKEN_ELSE;
"=="    {yylval.op = *yytext; return TOKEN_OPERATOR;}
"!="    {yylval.op = *yytext; return TOKEN_OPERATOR;}
[a-zA-Z_][a-zA-Z0-9_]* {yylval.name = _strdup(yytext); return TOKEN_ID;}
[-]?[0-9]+    {yylval.val = atoi(yytext); return TOKEN_NUMBER;}
[()=;]  {return *yytext;}
"<="    {yylval.op = *yytext; return TOKEN_OPERATOR;}
">="    {yylval.op = *yytext; return TOKEN_OPERATOR;}
[*/+-<>] {yylval.op = *yytext; return TOKEN_OPERATOR;}
[ \t\n] {/* suppress the output of the whitespaces from the input file to stdout */}
#.* {/* one-line comment */}

%%