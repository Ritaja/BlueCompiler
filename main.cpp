#include <assert.h>
#include <fstream>
#include <iostream>
#include "astgen.h"
#include "astexec.h"
#include "parser.tab.h"

extern int yyparse();
extern FILE *yyin;
extern AstElement* astDest;

int main()
{
    //yydebug = 0;
	//struct AstElement *a = new AstElement;
	FILE *fp ;
	fopen_s(&fp,"example7.txt","r");
	//std::cout<<fp;
	if (fp==NULL)
	{
		std::cout<<"error no file read!";
	}
	yyin=fp;
    yyparse();
    /* Q&D WARNING: in production code this assert must be replaced by
     * real error handling. */
    //assert(a);
    struct ExecEnviron* e = createEnv();
    execAst(e, astDest);
    freeEnv(e);
	/*int x = 23;
	while(x>0)
	{
	
		std::cout<<"program output:: "<<x*x<<std::endl;
		x=x-1;
	}*/
	
	int a;
	std::cin>>a;
    /* TODO: destroy the AST */
}