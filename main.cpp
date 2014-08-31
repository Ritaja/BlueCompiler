#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include "astgen.h"
#include "astexec.h"
#include "parser.tab.h"


extern int yyparse();
extern FILE *yyin;
extern AstElement* astDest;
void printPair(const std::pair<std::string, double > &p);



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
    
    assert(a);
    struct ExecEnviron* e = createEnv();
	setPropertySet(e,"lerp",3.4);
	setPropertySet(e,"strt_radius",34.6);
	setPropertySet(e,"endradius_",21.8);
	
    execAst(e, astDest);
    
	std::map<std::string,double> propertySet = getPropertySet(e);
	std::cout<<std::endl;
	std::cout<<"################## Property Set: ##################"<<std::endl;
	std::for_each(propertySet.begin(), propertySet.end(), printPair);
	freeEnv(e);
	int z;
	std::cin>>z;
    /* TODO: destroy the AST */
}

void printPair(const std::pair<std::string, double > &p)
	{
		std::cout << "Name: " << p.first << "\t";
		std::cout<<"Value: "<<p.second<<std::endl;
	}