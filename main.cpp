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
    //fopen_s(&fp,"example5.txt","r");
	fopen_s(&fp,"ClothoidIFCPL.txt","r");
	//fopen_s(&fp,"ClothoidTest.txt","r");
	//std::cout<<fp;
	if (fp==NULL)
	{
		std::cout<<"error no file read!";
	}
	yyin=fp;
    yyparse();
    
    assert(a);
    struct ExecEnviron* e = createEnv();
	//setPropertySet(e,"lerp",3.4);
	setPropertySet(e,"start",861.369007, 537.722476);
	setPropertySet(e,"clockwise",0);
	setPropertySet(e,"startCurvature",1.0/500);
	setPropertySet(e,"endCurvature",1.0/367);
	setPropertySet(e,"end",930.775243, 503.483954);
	setPropertySet(e,"length",77.500241);
    execAst(e, astDest);
    
	std::map<std::string,double> propertySet = getPropertySet(e);
	std::cout<<std::endl;
	std::cout<<"################## Property Set: ##################"<<std::endl;
	std::for_each(propertySet.begin(), propertySet.end(), printPair);
	freeEnv(e);
	int z;
	std::cin>>z;
}

void printPair(const std::pair<std::string, double > &p)
	{
		std::cout << "Name: " << p.first << "\t";
		std::cout<<"Value: "<<p.second<<std::endl;
	}