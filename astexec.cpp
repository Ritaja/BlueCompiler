#include "astexec.h"
#include "astgen.h"
#include <map>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

struct ExecEnviron
{
	/*  The stack for storing Numeric values from AST execution.  */
	std::map<std::string,double>var; 

	/* The stack for storing function mappings for calls later. */
	std::map<std::string,AstElement*>func;

	/* The stack for storing numeric arrays.reused as multi signature pass in function calls and dynamic print */
	std::vector<double>arr;

	/* reference to starting name */
	std::string varName;

	/* Simple map to store PropertySet mapings to provide as check to interface above. */
	std::map<std::string,double>propertySet; 
};

/* Forward declarations */
static int execTermExpression(struct ExecEnviron* e, struct AstElement* a);
static char* vectorPoscalc(std::string name,int i);
static char* vectorPoscalc(std::string name);
static int execBinExp(struct ExecEnviron* e, struct AstElement* a);
static void execAssign(struct ExecEnviron* e, struct AstElement* a);
static void execWhile(struct ExecEnviron* e, struct AstElement* a);
static void execCall(struct ExecEnviron* e, struct AstElement* a);
static void execStmt(struct ExecEnviron* e, struct AstElement* a);
static void execIf(struct ExecEnviron* e, struct AstElement* a);
static void execFunc(struct ExecEnviron* e, struct AstElement* a);
static void execfuncCall(struct ExecEnviron* e, struct AstElement* a);
static void execPrint(struct ExecEnviron* e, struct AstElement* a);
static int execArrSub(struct ExecEnviron* e,std::vector<int>arrLeft,std::vector<int>arrRight);
static int execArrAdd(struct ExecEnviron* e,std::vector<int>arrLeft,std::vector<int>arrRight);
static int execArray(struct ExecEnviron* e, struct AstElement* a);
static int execVector(struct ExecEnviron* e, struct AstElement* a);
static int execVectors(struct ExecEnviron* e, struct AstElement* a);
static int execVector2d(struct ExecEnviron* e, struct AstElement* a);
static int execVec1dEl(struct ExecEnviron* e, struct AstElement* a);
static int execVec2dEl(struct ExecEnviron* e, struct AstElement* a);
static void execVecAssign(struct ExecEnviron* e, struct AstElement* a);
static void execVec2dAssign(struct ExecEnviron* e, struct AstElement* a);
static void execElseIf(struct ExecEnviron* e, struct AstElement* a);
static void execRtrnByExp(struct ExecEnviron* e, struct AstElement* a);
static void destroyTemp(ExecEnviron* e, int length);
static void execFuncCallAssign(struct ExecEnviron* e, struct AstElement* a, std::vector<std::string>signatures);
static void execFuncAssign(struct ExecEnviron* e, struct AstElement* a);



/* Lookup Array for AST elements which yields values */
static int(*valExecs[])(struct ExecEnviron* e, struct AstElement* a) =
{
    execTermExpression,
    execTermExpression,
    execBinExp,
    NULL,
    NULL,
    NULL,
	NULL,
	NULL,
    NULL,
    NULL,
    NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	execVec1dEl,
	execVec2dEl,
	NULL,
	NULL,
	NULL,
	NULL
};

/* lookup array for non-value AST elements */
static void(*runExecs[])(struct ExecEnviron* e, struct AstElement* a) =
{
    NULL, /* ID and numbers are canonical and */
    NULL, /* don't need to be executed */
    NULL, /* a binary expression is not executed */
    execAssign,
    execWhile,
	execFunc,
	NULL, //reserved for execSignatures
	NULL,
    execCall,
    execStmt,
	execIf,
	execElseIf,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	execVecAssign,
	execVec2dAssign,
	execRtrnByExp,
	execFuncAssign
};

static int (*arrExecs[])(struct ExecEnviron* e, struct AstElement* a) =
{
    NULL, /* ID and numbers are canonical and */
    NULL, /* don't need to be executed */
    NULL, /* a binary expression is not executed */
	NULL,
    NULL,
	NULL,
	NULL, 
	NULL,
    NULL,
    NULL,
	NULL,
	NULL,
	execArray,
	execVector,
	execVectors,
	execVector2d,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

/* Dispatches any value expression */
static int dispatchExpression(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(valExecs[a->kind]);
	std::cout<<"dispatchExpression:: Array Lookup no: "<< a->kind<<std::endl;
    return valExecs[a->kind](e, a);
}

/* Dispatches any AST statement execution expression */
static void dispatchStatement(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
	std::cout<< "dispatchStatement:: Array Lookup no: "<<a->kind << runExecs[a->kind] << std::endl;
    assert(runExecs[a->kind]);
	std::cout<<" "<<std::endl;
    runExecs[a->kind](e, a);
}

/* Dispatches any array type expression and returns execution environment (or execution stack in theory)*/
static int dispatchArray(struct ExecEnviron* e, struct AstElement* a)
{
	assert(a);
	std::cout<<"dispatchArray:: "<<a->kind<<std::endl;
	assert(arrExecs[a->kind]);
	return arrExecs[a->kind](e, a);
}


static void onlyName(const char* name, const char* reference, const char* kind)
{
    if(strcmp(reference, name))
    {
        fprintf(stderr,
            "This language knows only the %s '%s', not '%s'\n",
            kind, reference, name);
        //exit(1);
    }
}

static void onlyX(const char* name)
{
    //onlyName(name, "x", "variable"); block for testing remove this
}

static void onlyPrint(const char* name)
{
    onlyName(name, "print", "function");
}

/* only called by statement when vectors are ncountered. and by print. first situ
takes care of print operation as it only can handle one term. Next for vector creations 
old code used temp array. Implementation is kept unchanged here in new interpreter.*/
static int execArray(struct ExecEnviron* e, struct AstElement* a)
{
	std::string varName;
	//need lookahead at caller
	if((a->data.array.element.size()) == 1)
	{
		/*the return here can be only the length as the varname set on environment holds
		and we only need to know how many places to traverse.*/
		int length = dispatchExpression(e,a->data.array.element[0]);
		return length;
	}
	else
	{
		for(int i=0;i<(a->data.array.element.size());i++)
		{
			std::cout<<"ArrayElement Size: "<<a->data.array.element.size()<<std::endl;
			int length = dispatchExpression(e,a->data.array.element[i]);
			varName = e->varName;
			std::cout<<"length is: "<<length<<std::endl;
			if(length == 1)
			{
				//As ingle value is returned. Could be a NUMBER or a VARIABLE returning a value. Store in temp.
				e->arr.resize(i+1);
				e->arr[i] = e->var[varName];
				std::cout<<"ExecArray value at "<<varName<<" is: "<<e->arr[i] <<std::endl;
			}
			else if(length > 1)
			{
				std::cout<<"Vector as a value to a vector element is not supported!!"<<std::endl;
			}
			if (length == NULL)
			{

				std::cout<<"No value found for variable: "<<a->data.array.element[i]<<std::endl;
           
			}
		
		}
		return 0;
	}
	
}


static int execVector(struct ExecEnviron* e, struct AstElement* a)
{
	assert(a->kind == AstElement::ekVector);
	//store the vector in the environment map named Vector
	dispatchArray(e,a->data.vector.array);
	
	std::string vectorName;

	for(int i=0;i<(e->arr.size());i++)
	{
		//store unique names in var by appending positions onto it
		vectorName = (a->data.vector.name)+std::to_string(i);
		e->var[vectorName] = e->arr[i];
		std::cout<<"ExecVector value at"<<vectorName<<"is: "<<e->var[vectorName] <<std::endl;

	}
	return NULL;
}

static int execVectors(struct ExecEnviron* e, struct AstElement* a)
{
	assert(a->kind == AstElement::ekVectors);
	for (int i=0;i<a->data.vectors.count;i++)
	{
		//set name and sent for execArray to process.
		
		std::string vectorPos2Intrmdt = (a->data.vectors.name)+std::to_string(i);
		char* vectorPos2 = new char[vectorPos2Intrmdt.size()+1];
		/*Microsoft stdext library dependant code. Or just turn off 
		compilation warnings and use std functions.*/
		std::copy(vectorPos2Intrmdt.begin(),vectorPos2Intrmdt.end(),stdext::checked_array_iterator<char*>(vectorPos2,vectorPos2Intrmdt.length()));
		vectorPos2[vectorPos2Intrmdt.size()]='\0';
		a->data.vectors.vector[i]->data.vector.name = vectorPos2;
		dispatchArray(e,a->data.vectors.vector[i]);
		std::string vectorPos;

		for(int j=0;j<(e->arr.size());j++)
		{
			//store unique names in var by appending positions onto it
			vectorPos = (vectorPos2Intrmdt)+std::to_string(j);
			e->var[vectorPos] = e->arr[j];
			std::cout<<"ExecVectors value at"<<vectorPos<<" is: "<<e->var[vectorPos] <<std::endl;

		}
	}
	

	return NULL;
}

static int execVector2d(struct ExecEnviron* e, struct AstElement* a)
{
	assert(a->kind == AstElement::ekVector2d);
	a->data.vector2d.vectors->data.vectors.name = a->data.vector2d.name;
	dispatchArray(e,a->data.vector2d.vectors);
	
	return NULL;
}


static int execTermExpression(struct ExecEnviron* e, struct AstElement* a)
{
    /* This function handles two different kinds of
     * AstElement. And is used for value lookup */
    assert(a);
    if(AstElement::ekNumber == a->kind)
    {
       		
	    e->varName = "Value";
		e->var["Value"] = (a->data.val);
		return 1;
    }
    else
    {
        if(AstElement::ekId == a->kind)
        {
            onlyX(a->data.name); //only checks if new variable is defined but obsolete in evolved code flow reutilise
            assert(e);
			//limitation vectors and variables should have different names.As in all programming languages.
			
			if( e->var.find( ((a->data.name)+std::to_string(0)) ) == e->var.end() && e->var.find( ((a->data.name)+std::to_string(0)+std::to_string(0)) ) == e->var.end() )
			{

				//No vectors exist. returns if of type ID as value is assumed to be defined.
				std::cout<<"execTermexp:: "<<e->var[(a->data.name)]<<"  ekID: "<<a->data.name<<std::endl;
				e->var[(a->data.name)];
				
				e->varName = a->data.name;
				return 1;

			}
			else if(e->var.find( ((a->data.name)+std::to_string(0)+std::to_string(0)) ) == e->var.end())
			{
				//vector found. Need to return values. Not a vector2d.
				
				int i=0;
				
				do
				{
					
					std::cout<<"execTermexpVector:: "<<e->var[((a->data.name)+std::to_string(i))]<<"  ekID: "<<a->data.name<<std::endl;
					i++;

				}while( e->var.find( ((a->data.name)+std::to_string(i)) ) != e->var.end() );

				e->varName = a->data.name;
				return (i);

			}
		   else
			{
				//vector 2D. do Operation.
				int i=0;
				int length = 1;
				std::string name = a->data.name;
				char* vectorPos2=vectorPoscalc(name,i);
				a->data.name = vectorPos2;
				//recurssive call to get back an array.
				execTermExpression(e,a);
				
				do
				{

					/*Here the value is stored in fashion: Sample01 -> (2) and Sample12 -> (6) 
					if the complete vector2d is Sample = (1,2,3),(4,5,6) So, stores named vectors of the 2D*/
					
					i++;
					vectorPos2 = vectorPoscalc(name,i);
					a->data.name = vectorPos2;
					if(e->var.find( ((a->data.name)+std::to_string(0)) ) != e->var.end())
					{
						length = execTermExpression(e,a);
					}
					else
					{
						break;
					}
					/*The end condition should check if Sample30 exists in e->var
					which stores the values flat out. see execvector2d for reference.*/
				}while(true);
				std::cout<<"Loop exit: "<<((a->data.name)+std::to_string(0))<<std::endl;
				a->data.name = vectorPoscalc(name);
				e->varName = a->data.name;
				std::cout<<"execTermExp: varName: "<<e->varName<<std::endl;
				return (i);
			}
        }
		
    }
    fprintf(stderr, "OOPS: tried to get the value of a non-expression(%d)\n", a->kind);

    //exit(1);
}



static char* vectorPoscalc(std::string name,int i)
{
	std::string vectorPos2Intrmdt = name+std::to_string(i);
    char* vectorPos2 = new char[vectorPos2Intrmdt.size()+1];
	std::copy(vectorPos2Intrmdt.begin(),vectorPos2Intrmdt.end(),stdext::checked_array_iterator<char*>(vectorPos2,vectorPos2Intrmdt.length()));
	vectorPos2[vectorPos2Intrmdt.size()]='\0';
	//a->data.name = vectorPos2;
	return vectorPos2;
}

static char* vectorPoscalc(std::string name)
{
	std::string vectorPos2Intrmdt = name;
    char* vectorPos2 = new char[vectorPos2Intrmdt.size()+1];
	std::copy(vectorPos2Intrmdt.begin(),vectorPos2Intrmdt.end(),stdext::checked_array_iterator<char*>(vectorPos2,vectorPos2Intrmdt.length()));
	vectorPos2[vectorPos2Intrmdt.size()]='\0';
	//a->data.name = vectorPos2;
	return vectorPos2;
}

static int execBinExp(struct ExecEnviron* e, struct AstElement* a)
{
    assert(AstElement::ekBinExpression == a->kind);
	std::cout<<"::execBinExp:: "<<std::endl;
	/*if null is returned on either side. it has a vector variable
	store it in temporary arrays to operate on them.*/
	std::string temp = "Temp";
	const int right = dispatchExpression(e, a->data.expression.right);
	std::string varNameRight = e->varName;
	double varRight;
	double placeHolder;
	if(varNameRight.compare("Value") == 0)
	{
		varRight = e->var[varNameRight];
	}
    const int left = dispatchExpression(e, a->data.expression.left);
	std::string varNameLeft = e->varName;
	/*Special case arises when we have 2+3 or two value comparisons
	evidently the execution environment stack for Value gets overiiten. 
	So, push of value is done for this scenario */ 
	
	
    
	
	if(right != left)
	{
		//size not equal unsupported operation
		std::cout<<"Size not equal on expression Terms. Unsupported operation!"<<std::endl;
	}

	else
	{
		
	switch(a->data.expression.op)
		{
			case '+':
				if(right ==1 && left ==1)
				{
					if(varNameLeft.compare("Value") == 0 && varNameRight.compare("Value") == 0)
					{
						e->var[temp] = varRight + e->var[(varNameLeft)];
						e->varName = temp;
						std::cout<<"AddedDouble "<<e->var[temp]<<" Left: "<<varNameLeft<<" Right: "<<varNameRight<<std::endl;
					}
					else
					{
						placeHolder = e->var[varNameLeft] + e->var[varNameRight];
						e->var[temp] = placeHolder;
						std::cout<<"Added "<<e->var[temp]<<" Left: "<<varNameLeft<<" "<<e->var[varNameLeft]<<" Right: "<<varNameRight<<" "<<e->var[varNameRight]<<" PlaceHolder: "<<placeHolder<<std::endl;
						e->varName = temp;
					}
					return 1;
				}
				else if( e->var.find(varNameLeft+std::to_string(0)+std::to_string(0)) != e->var.end() || e->var.find(varNameRight+std::to_string(0)+std::to_string(0)) != e->var.end() )
				{
					//found array 2d. names with Sample01, Sample11
					for(int i=0;i<left;i++)
					{
						int j=0;
						do
						{
							e->var[(temp+std::to_string(i)+std::to_string(j))] = e->var[(varNameLeft+std::to_string(i)+std::to_string(j))] + e->var[(varNameRight+std::to_string(i)+std::to_string(j))];
							j++;
						}while(e->var.find(varNameLeft+std::to_string(i)+std::to_string(j)) != e->var.end());
					}
		          //calculation done set variables and return.
				e->varName = temp;
				return left;
				}
				else if( e->var.find(varNameLeft+std::to_string(0)) != e->var.end() || e->var.find(varNameRight+std::to_string(0)) != e->var.end() )
				{
						//found 1d array. Names with Sample0 Sample1
						for(int i=0;i<left;i++)
						{
							e->var[(temp+std::to_string(i))] = e->var[(varNameLeft+std::to_string(i))] + e->var[(varNameRight+std::to_string(i))];
						}
					//calculation done set variables and return.
					e->varName = temp;
					return left;
				}
				else
				{
					std::cout<<"Unsupported operation!!Cant find stored value"<<std::endl;
				}
			case '-':
				if(right ==1 && left ==1)
				{
					if(varNameLeft.compare("Value") == 0 && varNameRight.compare("Value") == 0)
					{
						e->var[temp] =  e->var[(varNameLeft)] - varRight ;
						e->varName = temp;
						std::cout<<"AddedDouble "<<e->var[temp]<<" Left: "<<varNameLeft<<" "<<e->var[varNameLeft]<<" Right: "<<varRight<<std::endl;
					}
					else
					{
						placeHolder = e->var[varNameLeft] - e->var[varNameRight];
						e->var[temp] = placeHolder;
						std::cout<<"Added "<<e->var[temp]<<" Left: "<<varNameLeft<<" "<<e->var[varNameLeft]<<" Right: "<<varNameRight<<" "<<e->var[varNameRight]<<" PlaceHolder: "<<placeHolder<<std::endl;
						e->varName = temp;
					}
					return 1;
				}
				else if( e->var.find(varNameLeft+std::to_string(0)+std::to_string(0)) != e->var.end() || e->var.find(varNameRight+std::to_string(0)+std::to_string(0)) != e->var.end() )
				{
					//found array 2d. names with Sample01, Sample11
					for(int i=0;i<left;i++)
					{
						int j=0;
						do
						{
							e->var[(temp+std::to_string(i)+std::to_string(j))] = e->var[(varNameLeft+std::to_string(i)+std::to_string(j))] - e->var[(varNameRight+std::to_string(i)+std::to_string(j))];
							j++;
						}while(e->var.find(varNameLeft+std::to_string(i)+std::to_string(j)) != e->var.end());
					}
		          //calculation done set variables and return.
				e->varName = temp;
				return left;
				}
				else if( e->var.find(varNameLeft+std::to_string(0)) != e->var.end() || e->var.find(varNameRight+std::to_string(0)) != e->var.end() )
				{
						//found 1d array. Names with Sample0 Sample1
						for(int i=0;i<left;i++)
						{
							e->var[(temp+std::to_string(i))] = e->var[(varNameLeft+std::to_string(i))] - e->var[(varNameRight+std::to_string(i))];
						}
					//calculation done set variables and return.
					e->varName = temp;
					return left;
				}
				else
				{
					std::cout<<"Unsupported operation!!Cant find stored value"<<std::endl;
				}
			case '*':
				if(right ==1 && left ==1)
				{
					if(varNameLeft.compare("Value") == 0 && varNameRight.compare("Value") == 0)
					{
						e->var[temp] = varRight * e->var[(varNameLeft)];
						e->varName = temp;
						std::cout<<"AddedDouble "<<e->var[temp]<<" Left: "<<varNameLeft<<" Right: "<<varNameRight<<std::endl;
					}
					else
					{
						placeHolder = e->var[varNameLeft] * e->var[varNameRight];
						e->var[temp] = placeHolder;
						std::cout<<"Added "<<e->var[temp]<<" Left: "<<varNameLeft<<" "<<e->var[varNameLeft]<<" Right: "<<varNameRight<<" "<<e->var[varNameRight]<<" PlaceHolder: "<<placeHolder<<std::endl;
						e->varName = temp;
					}
					return 1;
				}
				else
				{
					std::cout<<"Unsupported operation for vector Type"<<std::endl;
				}
			case '<':
				if(right ==1 && left ==1)
				{
					return e->var[varNameLeft] < e->var[(varNameRight)];
					/*e->var[temp] = e->var[varNameLeft] < e->var[(varNameRight)];
					e->varName = temp;
					return 1;*/
				}
				else
				{
					std::cout<<"Unsupported operation for vector Type"<<std::endl;
				}
			case '>':
				if(right ==1 && left ==1)
				{
					return e->var[varNameLeft] > e->var[(varNameRight)];
					/*e->var[temp] = e->var[varNameLeft] > e->var[(varNameRight)];
					e->varName = temp;
					return 1;*/
				}
				else
				{
					std::cout<<"Unsupported operation for vector Type"<<std::endl;
				}
			case 'LE':
				if(right ==1 && left ==1)
				{
					return e->var[varNameLeft] <= e->var[(varNameRight)];
					/*e->var[temp] = e->var[varNameLeft] <= e->var[(varNameRight)];
					e->varName = temp;
					return 1;*/
				}
				else
				{
					std::cout<<"Unsupported operation for vector Type"<<std::endl;
				}
			case 'GE':
				if(right ==1 && left ==1)
				{
					return e->var[varNameLeft] >= e->var[(varNameRight)];
					/*e->var[temp] = e->var[varNameLeft] >= e->var[(varNameRight)];
					e->varName = temp;
					return 1;*/
				}
				else
				{
					std::cout<<"Unsupported operation for vector Type"<<std::endl;
				}
			case '==':
				if(right ==1 && left ==1)
				{
					return e->var[varNameLeft] == e->var[(varNameRight)];
					/*e->var[temp] = e->var[varNameLeft] == e->var[(varNameRight)];
					e->varName = temp;
					return 1;*/
				}
				else
				{
					std::cout<<"Unsupported operation for vector Type"<<std::endl;
				}
			case 'NE':
				if(right ==1 && left ==1)
				{
					std::cout<<"Not Equals called"<<std::endl;
					return e->var[varNameLeft] != e->var[(varNameRight)];
					/*e->var[temp] = e->var[varNameLeft] != e->var[(varNameRight)];
					e->varName = temp;
					return 1;*/
				}
				else
				{
					std::cout<<"Unsupported operation for vector Type"<<std::endl;
				}
			default:
				fprintf(stderr,  "OOPS: Unknown operator:%c\n", a->data.expression.op);
            //exit(1);
		}
	
	}
	
	
	
	
}


static void execAssign(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(AstElement::ekAssignment == a->kind);
    //onlyX(a->data.assignment.name);
    assert(e);
	struct AstElement* r = a->data.assignment.right;
	int length = dispatchExpression(e, r);
	std::string varName = e->varName;
	std::string assgnName = (a->data.assignment.name);


	if(length==1)
	{
		e->var[(assgnName)] = e->var[varName];
		std::cout<<"ExecAssign: "<<assgnName<<"Value: "<<e->var[(assgnName)];
		destroyTemp(e,1);
	}


	else if( e->var.find(varName+std::to_string(0)+std::to_string(0)) != e->var.end() )
	{
		//found array 2d. names with Sample01, Sample11
		for(int i=0;i<length;i++)
		{
			int j=0;
			do
			{
				e->var[(assgnName+std::to_string(i)+std::to_string(j))] = e->var[(varName+std::to_string(i)+std::to_string(j))];
			j++;
			}while(e->var.find(varName+std::to_string(i)+std::to_string(j)) != e->var.end());
		}
		 //assignment to environment stack done. Assign doesenot return anything. destroy temp in memory.
		destroyTemp(e,length);
	}
	else if( e->var.find(varName+std::to_string(0)) != e->var.end() )
	{
		//found 1d array. Names with Sample0 Sample1
		for(int i=0;i<length;i++)
		{
		     e->var[(assgnName+std::to_string(i))] = e->var[(varName+std::to_string(i))];
		}
		//assignment to environment stack done. Assign doesenot return anything.destroy temp in memory.
		destroyTemp(e,length);
	}
	else
	{
		std::cout<<"ExecAssign: Unsupported operation!!Cant find stored value"<<std::endl;
	}	
	
//end of assignment
}

/*This function is used to assign the values from a function return statement
into a variable. The variable does not need to be initialized with a data type.
The data type is selected automatically as per the return statement. The value 
for the variable is stored in the execution environment value lookup stack e->var*/
static void execFuncAssign(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
	assert(AstElement::ekFuncAssign == a->kind);
    //onlyX(a->data.assignment.name);
	
    assert(e);
	struct AstElement* r = a->data.assignment.right;
	execfuncCall(e, r);
	std::string varName = e->varName;
	std::string assgnName = (a->data.assignment.name);
	std::cout<<"execFuncAssign: varName: "<<varName<<std::endl;

	if(e->var.find(varName) != e->var.end())
	{
		//found expression of type number or token_id
		e->var[(assgnName)] = e->var[varName];
		std::cout<<"ExecFuncAssign: "<<assgnName<<"Value: "<<e->var[(assgnName)];
		destroyTemp(e,1);
	}


	else if( e->var.find(varName+std::to_string(0)+std::to_string(0)) != e->var.end() )
	{
		//found array 2d. names with Sample01, Sample11
		int i = 0;

		while ( e->var.find(varName+std::to_string(i)+std::to_string(0)) != e->var.end() )
		{
			int j=0;
			do
			{
				e->var[(assgnName+std::to_string(i)+std::to_string(j))] = e->var[(varName+std::to_string(i)+std::to_string(j))];
			j++;
			}while(e->var.find(varName+std::to_string(i)+std::to_string(j)) != e->var.end());

			i++;
		}
		 //assignment to environment stack done. Assign doesenot return anything. destroy temp in memory.
		destroyTemp(e,i);
	}
	else if( e->var.find(varName+std::to_string(0)) != e->var.end() )
	{
		//found 1d array. Names with Sample0 Sample1
		int i = 0;
		while( e->var.find(varName+std::to_string(i)) != e->var.end() )
		{
		     e->var[(assgnName+std::to_string(i))] = e->var[(varName+std::to_string(i))];
			 i++;
		}
		//assignment to environment stack done. Assign doesenot return anything.destroy temp in memory.
		destroyTemp(e,i);
	}
	else
	{
		std::cout<<"ExecFuncAssign: Unsupported operation!!Cant find stored value"<<std::endl;
	}	
	
//end of assignment
}


/*This function does the clean up of values stored as 'Temp' or 'Value'
in memory. Free up memory space. Should be called after each assignment
operation*/
static void destroyTemp(ExecEnviron* e, int length)
{
	std::string varName = "Temp";
	std::string valName = "Value";

	if( e->var.find(varName) != e->var.end() && length == 1)
	{
		e->var.erase(varName);
		
	}


	else if( e->var.find(varName+std::to_string(0)+std::to_string(0)) != e->var.end() )
	{
		//found array 2d. names with Sample01, Sample11
		for(int i=0;i<length;i++)
		{
			int j=0;
			do
			{
			   e->var.erase(varName+std::to_string(i)+std::to_string(j));
			j++;
			}while(e->var.find(varName+std::to_string(i)+std::to_string(j)) != e->var.end());
		}
		 //assignment to environment stack done. Assign doesenot return anything
		
	}
	else if( e->var.find(varName+std::to_string(0)) != e->var.end() )
	{
		//found 1d array. Names with Sample0 Sample1
		for(int i=0;i<length;i++)
		{
		     e->var.erase(varName+std::to_string(i));
		}
		//assignment to environment stack done. Assign doesenot return anything
		
	}
	//else destroy value
	else if(e->var.find(valName) != e->var.end() && length == 1)
	{
		e->var.erase(valName);
	}
	else
	{
		std::cout<<"ExecAssign: Unsupported operation!!Can't find temp or value"<<std::endl;
	}	
	
}


//a[i] = expr
static void execVecAssign(struct ExecEnviron* e, struct AstElement* a)
{
	std::string vectorname = a->data.VecAssignment.name;
	int pos = a->data.VecAssignment.elementPos1;

	e->var[(vectorname+std::to_string(pos))] = dispatchExpression(e,a->data.VecAssignment.right);
}

static void execVec2dAssign(struct ExecEnviron* e, struct AstElement* a)
{
	//TODO::
}

static void execWhile(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(AstElement::ekWhile == a->kind);
    struct AstElement* const c = a->data.whileStmt.cond;
    struct AstElement* const s = a->data.whileStmt.statements;
    assert(c);
    assert(s);
    while(dispatchExpression(e, c))
    {
        dispatchStatement(e, s);
    }
}

//support for the if statement
static void execIf(struct ExecEnviron* e, struct AstElement* a)
{
	assert(a);
	assert(AstElement::ekIf == a->kind);
	struct AstElement* const c = a->data.ifStatement.cond;
	struct AstElement* const t = a->data.ifStatement.ifTrue;
	struct AstElement* const f = a->data.ifStatement.ifFalse;

	//current code returns 1 or 0 for comparison operators. As these are not defined for the vector types.
	if(dispatchExpression(e, c))
	{
		dispatchStatement(e, t);
	}
	else
	{
		dispatchStatement(e, f);
	}
}

//support for the else if statement
static void execElseIf(struct ExecEnviron* e, struct AstElement* a)
{
	assert(a);
	assert(AstElement::ekElseIf == a->kind);
	struct AstElement* const ic = a->data.elseifStatement.ifcond;
	struct AstElement* const it = a->data.elseifStatement.ifTrue;
	struct AstElement* const ec = a->data.elseifStatement.elseifCond;
	struct AstElement* const ect = a->data.elseifStatement.elseifCondTrue;
	struct AstElement* const ecf = a->data.elseifStatement.elseifCondFalse;

	//current code returns 1 or 0 for comparison operators. As these are not defined for the vector types.
	if(dispatchExpression(e, ic))
	{
		dispatchStatement(e, it);
	}
	else if(dispatchExpression(e, ec))
	{
		dispatchStatement(e, ect);
	}
	else
	{
		dispatchStatement(e, ecf);
	}
}

static void execCall(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(AstElement::ekCall == a->kind);
	std::string callName = a->data.call.name;
	std::cout<<"the fuction called is : "<< callName << std::endl;
	if ((callName).compare("print") != 0)
	{
		execfuncCall(e,a);
	}
	else if ((callName).compare("print") == 0)
	{
		execPrint(e,a);
	}
	else 
	{
		//logic for handling errorneous calls (undef-funcs and non print stmnts)
		onlyPrint(a->data.call.name); //here should be check for called type
	}
}


static void execfuncCall(struct ExecEnviron* e, struct AstElement* a)
{
	AstElement* funcExec = e->func[a->data.call.name];
	std::vector <std::string> signatures ;

//loop through all the signatures one by one
for (int i=0;i<(funcExec->data.func.signatures->data.signatures.signature.size());i++) 
	{
		signatures.resize(i+1);
		//set asignment name and variable name to lookup from.
		std::string varName = e->varName;
		
		 signatures[i] = (funcExec->data.func.signatures->data.signatures.signature[i]->data.signature.assignment->data.assignment.name);
		 std::cout<<"Signature:: "<<signatures[i]<<std::endl;
	
	//end of assignment
    }
execFuncCallAssign(e,a->data.call.param,signatures);
dispatchStatement(e,funcExec->data.func.statements);

}


static void execFuncCallAssign(struct ExecEnviron* e, struct AstElement* a, std::vector<std::string>signatures)
{
	std::string varName;
	std::string assgnName;
	  
		for(int i=0;i<(a->data.array.element.size());i++)
		{
			std::cout<<"Signature Size: "<<a->data.array.element.size()<<std::endl;
			int length = dispatchExpression(e,a->data.array.element[i]);
			varName = e->varName;
			assgnName = signatures[(signatures.size()-(i+1))];
			std::cout<<"execfuncCallAssign: length is: "<<length<<std::endl;
			if(length==1)
			{
				e->var[(assgnName)] = e->var[varName];
				std::cout<<"ExecFuncCallAssign: "<<assgnName<<" Value: "<<e->var[(assgnName)]<<std::endl;
				destroyTemp(e,1);
			}


			else if( e->var.find(varName+std::to_string(0)+std::to_string(0)) != e->var.end() )
			{
				//found array 2d. names with Sample01, Sample11
				for(int i=0;i<length;i++)
				{
					int j=0;
					do
					{
						e->var[(assgnName+std::to_string(i)+std::to_string(j))] = e->var[(varName+std::to_string(i)+std::to_string(j))];
					j++;
					}while(e->var.find(varName+std::to_string(i)+std::to_string(j)) != e->var.end());
				}
				//assignment to environment stack done. Assign doesenot return anything. destroy temp in memory.
				destroyTemp(e,length);
			}
			else if( e->var.find(varName+std::to_string(0)) != e->var.end() )
			{
				//found 1d array. Names with Sample0 Sample1
				for(int i=0;i<length;i++)
				{
					e->var[(assgnName+std::to_string(i))] = e->var[(varName+std::to_string(i))];
				}
				//assignment to environment stack done. Assign doesenot return anything.destroy temp in memory.
				destroyTemp(e,length);
			}
			else
			{
				std::cout<<"ExecAssign: Unsupported operation!!Cant find stored value"<<std::endl;
			}	
		}
	
}

/* block for print execution */
static void execPrint(struct ExecEnviron* e, struct AstElement* a)
{
	//logic for non func call, print in our case
	std::cout<<"execPrint:: param for the print statement: "<< a->data.call.param << std::endl;
	
	//ExecEnviron* eTmp = dispatchArray(e, a->data.call.param);
	int length = dispatchArray(e, a->data.call.param);
	std::string varName = e->varName;
	std::cout<<"execPrint:: length: "<< length << "varName" << varName<< std::endl;
	


	if(length==1)
	{
		std::cout<<e->var[varName]<<std::endl;
	}

	else if( e->var.find(varName+std::to_string(0)+std::to_string(0)) != e->var.end() )
	{
		//found array 2d. names with Sample01, Sample11
		for(int i=0;i<length;i++)
		{
			int j=0;
			do
			{
				std::cout<<e->var[(varName+std::to_string(i)+std::to_string(j))]<<" ";
			j++;
			}while(e->var.find(varName+std::to_string(i)+std::to_string(j)) != e->var.end());
			std::cout<<std::endl;
		}
		 //assignment to environment stack done. Assign doesenot return anything
		
	}
	else if( e->var.find(varName+std::to_string(0)) != e->var.end() )
	{
		//found 1d array. Names with Sample0 Sample1
		for(int i=0;i<length;i++)
		{
		    std::cout<<e->var[(varName+std::to_string(i))]<<" ";
		}
		std::cout<<std::endl;
		//assignment to environment stack done. Assign doesenot return anything
		
	}
	else
	{
		std::cout<<"Unsupported operation!!Cant find stored value"<<std::endl;
	}
	
}

//not req chng
static void execStmt(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(AstElement::ekStatements == a->kind);
    int i;
	//traversing of the aray of structures for each statement now its vectors
	for(i=0; i < (a->data.statements.statements.size()); i++)
    {
		std::cout<<"execStmnt:: size"<<a->data.statements.statements.size()<<std::endl;
		//operator != is not defined for enim types in c++ as of now BE careful
		if(a->data.statements.statements[i]->kind == AstElement::ekArray||a->data.statements.statements[i]->kind == AstElement::ekVector||a->data.statements.statements[i]->kind == AstElement::ekVectors||a->data.statements.statements[i]->kind == AstElement::ekVector2d)
		{
			std::cout<<"execStmnt:KindNext "<<typeid(a->data.statements.statements[i]->kind).name()<<std::endl;
		    dispatchArray(e, a->data.statements.statements[i]);
		}
		else
		{
			dispatchStatement(e, a->data.statements.statements[i]);
			std::cout<<"execStmnt:KindNext "<<a->data.statements.statements[i]->kind<<std::endl;
		}
    }
}

//not req chng
//responsible for storing the function address in the map.. execution should be done in a separate map
static void execFunc(struct ExecEnviron* e, struct AstElement* a)
{
	assert(a);
	assert(AstElement::ekFunc == a->kind);
	std::stringstream ss;
	ss << (a->data.func.signatures);
	char* fname = (a->data.func.name);
	std::cout<<"the fuction map value is: "<< fname << std::endl;
	//func namer is the key and the value is pointer to function structure (entire astelement)
	e->func[fname]= a;
	std::cout << "Value entered for key "<< e->func[fname] << std::endl;
	
}

/* vector1d element lookup and return from the e->var stack. */
static int execVec1dEl(struct ExecEnviron* e, struct AstElement* a)
{
	std::string vectorName = a->data.Vec1delement.name;
	int pos = a->data.Vec1delement.elementPos;
	return e->var[(vectorName+std::to_string(pos))];
}

/* vector2d element lookup and return the e->var at the specified position. 
This is the execution stack. */
static int execVec2dEl(struct ExecEnviron* e, struct AstElement* a)
{
	/*std::string vectorName = a->data.Vec1delement.name;
	int pos = a->data.Vec1delement.elementPos;
	return e->var[(vectorName+std::to_string(pos))];*/
	return NULL;
}


static void execRtrnByExp(struct ExecEnviron* e, struct AstElement* a)
{
	dispatchExpression(e,a->data.returnData.exp);
	/*int length = dispatchExpression(e,a->data.returnData.exp);
	std::cout<<"wexecRtrnByValue"<<std::endl;
	return length;*/
}





void execAst(struct ExecEnviron* e, struct AstElement* a)
{

        dispatchStatement(e, a);
	
}


void setPropertySet(struct ExecEnviron* e,char* varName, double val)
{
	e->var[varName] = val;
	e->propertySet[varName] = val;
}

void setPropertySet(struct ExecEnviron* e,char* varName, double val0, double val1)
{
	e->var[varName+std::to_string(0)] = val0;
	e->propertySet[varName+std::to_string(0)] = val0;

	e->var[varName+std::to_string(1)] = val1;
	e->propertySet[varName+std::to_string(1)] = val1;
}

double getPropertySet(struct ExecEnviron* e, char* varName)
{
	return e->var[varName];
}


std::map<std::string,double> getPropertySet(struct ExecEnviron* e)
{
	return e->propertySet;
}

struct ExecEnviron* createEnv()
{ 
	return new ExecEnviron();
}

void freeEnv(struct ExecEnviron* e)
{
    free(e);
}