%error-verbose /* instruct bison to generate verbose error messages*/
%{
#include "astgen.h"
#define YYDEBUG 1

/* Since the parser must return the AST, it must get a parameter where
 * the AST can be stored. The type of the parameter will be void*. */
struct AstElement* astDest;
extern int yylex();
%}

%union {
    int val;
    char op;
    char* name;
    struct AstElement* ast; /* this is the new member to store AST elements */
}

%token TOKEN_BEGIN TOKEN_END TOKEN_WHILE TOKEN_DO
%token<name> TOKEN_ID
%token<val> TOKEN_NUMBER
%token<op> TOKEN_OPERATOR
%type<ast> program block statements statement assignment expression whileStmt call
%start program

%{
/* Forward declarations */
void yyerror(const char* const message);


%}

%%

program: statement';' { astDest = $1; };

block: TOKEN_BEGIN statements TOKEN_END{ $$ = $2; };

statements: {$$=0;}
    | statements statement ';' {$$=makeStatement($1, $2);}
    | statements block';' {$$=makeStatement($1, $2);};

statement: 
      assignment {$$=$1;}
    | whileStmt {$$=$1;}
    | block {$$=$1;}
    | call {$$=$1;}

assignment: TOKEN_ID '=' expression {$$=makeAssignment($1, $3);}

expression: TOKEN_ID {$$=makeExpByName($1);}
    | TOKEN_NUMBER {$$=makeExpByNum($1);}
    | expression TOKEN_OPERATOR expression {$$=makeExp($1, $3, $2);}

whileStmt: TOKEN_WHILE expression TOKEN_DO statement{$$=makeWhile($2, $4);};

call: TOKEN_ID '(' expression ')' {$$=makeCall($1, $3);};

%%

#include "astexec.h"
#include <stdlib.h>

void yyerror(const char* const message)
{
    fprintf(stderr, "Parse error:%s\n", message);
    exit(1);
}

