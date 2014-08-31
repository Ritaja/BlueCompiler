#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include "astgen.h"
#include "astexec.h"
#include "parser.tab.h"

extern int yyparse();
extern FILE *yyin;
extern AstElement* astDest;

int main()
{
    //yydebug = 0;
	struct AstElement *a = new AstElement;
	FILE *fp ;
	fopen_s(&fp,"example5.txt","r");
	//std::cout<<fp;
	if (fp==NULL)
	{
		std::cout<<"error no file read!";
	}
	yyin=fp;
    yyparse();
    /* Q&D WARNING: in production code this assert must be replaced by
     * real error handling. */
    assert(a);
    struct ExecEnviron* e = createEnv();
    execAst(e, astDest);
    freeEnv(e);
	/*int x = 23;
	while(x>0)
	{
	
		std::cout<<"program output:: "<<x*x<<std::endl;
		x=x-1;
	}*/

	//std::map<char,int> mymap;
 //   std::map<char,int>::iterator it;

 // // insert some values:
 // mymap['a']=10;
 // mymap['b']=20;
 // mymap['c']=30;
 // mymap['d']=40;
 // mymap['e']=50;
 // mymap['f']=60;

 // it=mymap.find('b');
 // mymap.erase (it);                   // erasing by iterator

 // mymap.erase ('c');                  // erasing by key

 // if(mymap.find('e') != mymap.end())
 // {
	//  std::cout<<"Found"<<mymap['e']<<std::endl;
 // }
	
	int z;
	std::cin>>z;
    /* TODO: destroy the AST */
}