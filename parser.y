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
    double val;
    char op;
    char* name;
    struct AstElement* ast; /* this is the new member to store AST elements */
}

%token TOKEN_BEGIN TOKEN_END TOKEN_WHILE TOKEN_DO BOX_OPEN BOX_CLOSE
%token TOKEN_IF TOKEN_ELSE TOKEN_COMMA TOKEN_VECTOR TOKEN_RETURN
%token TOKEN_POW TOKEN_FACTORIAL TOKEN_ACOS TOKEN_SQRT 
%token TOKEN_ROTATEZ TOKEN_MAGNITUDESQR TOKEN_TRANSFORM
%token TOKEN_VECTOR2d
%token<name> TOKEN_ID
%token<val> TOKEN_NUMBER
%token<op> TOKEN_OPERATOR
%type<ast> program block statements statement assignment expression whileStmt call ifStmt func 
%type<ast> signature signatures array vector vector2d vectors return
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
	| ifStmt{$$=$1;}
    | block {$$=$1;}
    | call {$$=$1;}
	| func {$$=$1;}
	| vector {$$=$1;}
	| vector2d {$$=$1;}
	| return {$$=$1;}

assignment: TOKEN_ID '=' expression {$$=makeAssignment($1, $3);}
          | TOKEN_ID {$$=makeAssignment($1);};
		  | TOKEN_ID BOX_OPEN TOKEN_NUMBER BOX_CLOSE '=' expression {$$=makeVecAssignment($1, $3, $6);}
		  | TOKEN_ID BOX_OPEN TOKEN_NUMBER BOX_CLOSE BOX_OPEN TOKEN_NUMBER BOX_CLOSE '=' expression {$$=makeVec2dAssignment($1, $3, $6, $9);}
		  | TOKEN_ID '=' call{$$=makeFuncAssignment($1, $3);}
                    
/* see how to support x=a[]+b[]*/
expression: TOKEN_ID {$$=makeExpByName($1);}
    | TOKEN_NUMBER {$$=makeExpByNum($1);}
    | expression TOKEN_OPERATOR expression {$$=makeExp($1, $3, $2);}
	| TOKEN_ID BOX_OPEN TOKEN_NUMBER BOX_CLOSE {$$=makeVec1delement($1, $3);};
	| TOKEN_ID BOX_OPEN TOKEN_NUMBER BOX_CLOSE BOX_OPEN TOKEN_NUMBER BOX_CLOSE {$$=makeVec2delement($1, $3, $6);};
	| TOKEN_POW '(' expression TOKEN_COMMA expression ')'{$$=makePow($3, $5);};
	| TOKEN_FACTORIAL '(' expression ')'{$$=makeFact($3);};
	| TOKEN_ACOS '(' expression ')'{$$=makeAcos($3);};
	| TOKEN_SQRT '(' expression ')'{$$=makeSqrt($3);};
	| TOKEN_ROTATEZ '(' expression ')'{$$=makeRotatez($3);};
	| TOKEN_MAGNITUDESQR '(' expression ')'{$$=makeMagnitudesqr($3);};
	| TOKEN_TRANSFORM '(' expression ')'{$$=makeTransform($3);};

array: {$$=0;}
     | array TOKEN_COMMA expression {$$=makeArray($1,$3);}
	 | array expression  {$$=makeArray($1,$2);}

vector: TOKEN_VECTOR TOKEN_ID '=' '(' array ')' {$$=makeVector($2,$5);}
      | TOKEN_VECTOR TOKEN_ID '=' '(' ')' {$$=makeNullVector($2);}
	  | TOKEN_VECTOR TOKEN_ID '=' expression {$$=makeAssignment($2, $4);}

vector2d: TOKEN_VECTOR2d TOKEN_ID '=' BOX_OPEN vectors BOX_CLOSE {$$=makeVector2d($2,$5);}
        | TOKEN_VECTOR2d TOKEN_ID '=' expression {$$=makeAssignment($2, $4);}

/*check null vectors creation*/
vectors: {$$=0;}
       | vectors TOKEN_COMMA '(' array ')' {$$=makeVectors($1,$4);}
       | vectors '(' array ')' {$$=makeVectors($1,$3);}
	   | vectors '('  ')' {$$=makeNullVectors();}


whileStmt: TOKEN_WHILE '(' expression ')' TOKEN_DO statement{$$=makeWhile($3, $6);};

/*if multiple else if support is required create a separate structure*/
ifStmt: TOKEN_IF  '(' expression ')' TOKEN_DO statement TOKEN_ELSE TOKEN_DO statement{$$=makeIf($3, $6, $9);};
      | TOKEN_IF  '(' expression ')' TOKEN_DO statement TOKEN_ELSE  TOKEN_IF '(' expression ')' TOKEN_DO statement TOKEN_ELSE TOKEN_DO statement{$$=makeElseIf($3, $6, $10, $13, $16);};

func: TOKEN_ID TOKEN_ID '(' signatures ')' statement {$$=makeFunc($2, $4, $6);};

signatures: {$$=0;}
          |signature signatures{$$=makeSignatures($2,$1);}
		  |'(' ')'{;};

signature: 
         TOKEN_COMMA TOKEN_ID assignment{$$=makeSignature($2,$3);} 
         |TOKEN_ID assignment{$$=makeSignature($1,$2);}


call: TOKEN_ID '(' array ')' {$$=makeCall($1, $3);};

return: TOKEN_RETURN expression{$$=makeReturnByExp($2);} 


%%

#include "astexec.h"
#include <stdlib.h>

void yyerror(const char* const message)
{
    fprintf(stderr, "Parse error:%s\n", message);
    //exit(1);
}

