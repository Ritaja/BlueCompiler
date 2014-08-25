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
	/* Obsolete structure */
    int x; 

	/*  The stack for storing Numeric values from AST execution.  */
	std::map<std::string,int>var; 

	/* The stack for storing function mappings for calls later. */
	std::map<std::string,AstElement*>func;

	/* The stack for storing numeric arrays.reused as multi signature pass in function calls and dynamic print */
	std::vector<int>arr;

	/* The stack to store arrays. */
	std::map<std::string,std::vector<int>>vectorArr;

	/* the FLAG signalls the presence of vector. Hence Reserved. */
	bool isVector2d; 

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
static ExecEnviron* execArray(struct ExecEnviron* e, struct AstElement* a);
static ExecEnviron* execVector(struct ExecEnviron* e, struct AstElement* a);
static ExecEnviron* execVectors(struct ExecEnviron* e, struct AstElement* a);
static ExecEnviron* execVector2d(struct ExecEnviron* e, struct AstElement* a);
static void display(std::string name,ExecEnviron* e);


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
	NULL,
	NULL,
	NULL,
	NULL
};

static ExecEnviron* (*arrExecs[])(struct ExecEnviron* e, struct AstElement* a) =
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
	execArray,
	execVector,
	execVectors,
	execVector2d
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
	std::cout<< "Array Lookup no: "<<a->kind << runExecs[a->kind] << std::endl;
    assert(runExecs[a->kind]);
	std::cout<<"dispatchStatement:: "<<std::endl;
    runExecs[a->kind](e, a);
}

/* Dispatches any array type expression and returns execution environment (or execution stack in theory)*/
static ExecEnviron* dispatchArray(struct ExecEnviron* e, struct AstElement* a)
{
	assert(a);
	std::cout<<"dispatchArray:: "<<a->kind;
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

static ExecEnviron* execArray(struct ExecEnviron* e, struct AstElement* a)
{
	//clear array before processing, if array is already filled
	if (e->arr.size() != 0)
	{
	e->arr.clear();
	}
	
	//fill array elements from AST
	for(int i=0;i<(a->data.array.element.size());i++)
	{
		std::cout<<"Size: "<<a->data.array.element.size()<<std::endl;
		e->arr.resize(i+1);
		//set the vector element position to lookup (1D).
		//e->vectorPosition1 = i;
		int value = dispatchExpression(e,a->data.array.element[i]);
		std::cout<<"Value is: "<<value<<std::endl;
		//vector encountered. array is processed further down AST. Logic to check NULL is tricky.
		if (value != NULL)
		{
			e->arr[i]= value;
		std::cout<<"Value at: "<<i<<"is "<<e->arr[i]<<std::endl;
           
		}
		
	}
	std::cout<<"isVector2d "<<e->isVector2d<<std::endl;
	return e;
}

static ExecEnviron* execVector(struct ExecEnviron* e, struct AstElement* a)
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
	return e;
}

static ExecEnviron* execVectors(struct ExecEnviron* e, struct AstElement* a)
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
	

	return e;
}

static ExecEnviron* execVector2d(struct ExecEnviron* e, struct AstElement* a)
{
	assert(a->kind == AstElement::ekVector2d);
	a->data.vector2d.vectors->data.vectors.name = a->data.vector2d.name;
	dispatchArray(e,a->data.vector2d.vectors);
	
	return e;
}

static int execTermExpression(struct ExecEnviron* e, struct AstElement* a)
{
    /* This function handles two different kinds of
     * AstElement.  */
    assert(a);
    if(AstElement::ekNumber == a->kind)
    {
        
		std::cout<<"execTermexp:: value "<<a->data.val<<std::endl;
		e->isVector2d = false;
		return (a->data.val);
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
				int value = e->var[(a->data.name)];
				e->isVector2d = false;
				return value;

			}
			else if(e->var.find( ((a->data.name)+std::to_string(0)+std::to_string(0)) ) == e->var.end())
			{
				//vector found. Need to return values. Not a vector2d.
				
				int i=0;
				
				do
				{
					e->arr.resize(i+1);
					e->arr[i]=e->var[((a->data.name)+std::to_string(i))];
					std::cout<<"execTermexpVector:: "<<e->arr[i]<<"  ekID: "<<a->data.name<<std::endl;
					i++;

				}while( e->var.find( ((a->data.name)+std::to_string(i)) ) != e->var.end() );

				e->isVector2d=false;
				return NULL;

			}
		   else
			{
				//vector 2D. do Operation.
				int i=0;
				std::string name = a->data.name;
				char* vectorPos2=vectorPoscalc(name,i);
				a->data.name = vectorPos2;
				//recurssive call to get back an array.
				execTermExpression(e,a);
				
				do
				{

					/*Here the value is stored in fashion: Sample1 -> (1,2,3) and Sample2 -> (4,5,6) 
					if the complete vector2d is Sample = (1,2,3),(4,5,6) So, stores named vectors of the 2D*/
					e->vectorArr[(vectorPos2)] = e->arr; 
					std::cout<<"execVector2D:value at "<<vectorPos2<<"is ";
					std::vector<int>arrTemp = e->vectorArr[(vectorPos2)];
					std::copy(arrTemp.begin(), arrTemp.end(), std::ostream_iterator<int>(std::cout, " "));
					std::cout<<" "<<std::endl;
					i++;
					vectorPos2 = vectorPoscalc(name,i);
					a->data.name = vectorPos2;
					if(e->var.find( ((a->data.name)+std::to_string(0)) ) != e->var.end())
					{
						execTermExpression(e,a);
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
				e->isVector2d=true;
				return NULL;
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
	/*if null is returned on either side. it has a vector variable
	store it in temporary arrays to operate on them.*/
	std::vector<int>arrLeft;
	std::vector<int>arrRight;
    const int left = dispatchExpression(e, a->data.expression.left);
	/*if(left == NULL && e->isVector2d)
	{
		std::string nameLeft = a->data.expression.left->data.name;
	}*/
	if (left == NULL)
	{
		std::cout<<"Here"<<std::endl;
		arrLeft = e->arr;
	}
    const int right = dispatchExpression(e, a->data.expression.right);
	/*if(left == NULL && e->isVector2d)
	{
		std::string nameRight = a->data.expression.right->data.name;
	}*/
	if (left == NULL)
	{
		std::cout<<"Here"<<std::endl;
		arrRight = e->arr;
	}
	if(left == NULL && right == NULL)
	{
		/*vector encountered. Support Operation.As with all languages 
		vetor * and / are impossible to support out of the box.*/
		switch(a->data.expression.op)
		{
			case '+':
            return execArrAdd(e,arrLeft,arrRight);
			case '-':
			return execArrSub(e,arrLeft,arrRight);
			default:
            fprintf(stderr,  "OOPS: Unknown operator for vector:%c\n", a->data.expression.op);
		}
	}
    switch(a->data.expression.op)
    {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '<':
            return left < right;
        case '>':
            return left > right;
		case '<=':
			return left <= right;
		case '>=':
			return left >= right;
		case '==':
			return left == right;
		case '!=':
			return left != right;
        default:
            fprintf(stderr,  "OOPS: Unknown operator:%c\n", a->data.expression.op);
            //exit(1);
    }
    /* no return here, since every switch case returns some value (or bails out) */
	
}

static int execArrAdd(struct ExecEnviron* e,std::vector<int>arrLeft,std::vector<int>arrRight)
{
	if(arrLeft.size() == arrRight.size())
	{
		for(int i=0;i<arrLeft.size();i++)
		{
			e->arr[i] = arrLeft[i] + arrRight[i];
		}
	}
	else
		fprintf(stderr,  "Size of vectors to be added are not identical!:%d\t%d\n",arrLeft.size(),arrRight.size());

	return NULL;
}

static int execArrSub(struct ExecEnviron* e,std::vector<int>arrLeft,std::vector<int>arrRight)
{
	if(arrLeft.size() == arrRight.size())
	{
		for(int i=0;i<arrLeft.size();i++)
		{
			e->arr[i] = arrLeft[i] - arrRight[i];
		}
	}
	else
     fprintf(stderr,  "Size of vectors to be added are not identical!");

	return NULL;
}

static void execAssign(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(AstElement::ekAssignment == a->kind);
    onlyX(a->data.assignment.name);
    assert(e);
    struct AstElement* r = a->data.assignment.right;
    //this line fucked me!
	int value = dispatchExpression(e, r);
	if (value != NULL)
	{
		//Not a vector assignment. do a normal one. 
		e->var[(a->data.assignment.name)] = dispatchExpression(e, r); 
	}
	else if (e->isVector2d)
	{
		//vector 2D assignment. Do operation.
		std::string vectorName=(a->data.assignment.name);
		/*get the TOKEN_ID that is passed for assignment. right has expression and 
		for TOKEN_ID expression stores name is data.name.Ref: makeExpByName()*/
		std::string refName=(a->data.assignment.right->data.name);

		int i=0;
		//store unique names in var by appending positions onto it
		std::string vectorNameVar = vectorName+std::to_string(i);
		std::string refNameVar = refName+std::to_string(i);
		
		do
		{
			std::vector<int>arrTemp = e->vectorArr[refNameVar];
			
			//create the array in base structure e->var for future ref (Need to create underlying arrays)
			for(int j=0;j<arrTemp.size();j++)
			{
				e->var[(vectorNameVar+std::to_string(j))] = arrTemp[j];
			}

			e->vectorArr[vectorNameVar] = arrTemp;
			
			i++;
			//update both
			vectorNameVar = vectorName+std::to_string(i);
			refNameVar = refName+std::to_string(i);
			
		}while(e->vectorArr.find(refNameVar) != e->vectorArr.end());
		
	}

	else
	{
		//vector 1D assignment. Do operation.
		std::string vectorName;
		
		for(int i=0;i<(e->arr.size());i++)
		{
			//store unique names in var by appending positions onto it
			vectorName = (a->data.assignment.name)+std::to_string(i);
			e->var[vectorName] = e->arr[i];

		}
	}
	
}

static void display(std::string name,ExecEnviron* e)
{
	
	int i = 0;
	char* vectorPos = vectorPoscalc(name,i);
	do
		{
			std::vector<int>arrTemp = e->vectorArr[(vectorPos)];
			std::copy(arrTemp.begin(), arrTemp.end(), std::ostream_iterator<int>(std::cout, " "));
			std::cout<<" "<<std::endl;
			i++;
			vectorPos = vectorPoscalc(name,i);
		}while( e->vectorArr.find(vectorPos) != e->vectorArr.end());
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
	if(dispatchExpression(e, c))
	{
		dispatchStatement(e, t);
	}
	else
	{
		dispatchStatement(e, f);
	}
}

//Need to refactor print handler to update new capabilities.
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
	std::cout<<"execfuncCall:: function called by name: "<< a->data.call.name << std::endl;
	AstElement* funcExec = e->func[a->data.call.name];
	std::cout<<"execfuncCall:: param passed: "<< a->data.call.param << " Number: " << a->data.call.param->kind << std::endl;
	ExecEnviron* eTmp = dispatchArray(e, a->data.call.param);
	for (int i=0;i<(funcExec->data.func.signatures->data.signatures.signature.size());i++) //check here for access violation
	{
		//most complicated line written. Take deep breath and break down in parts to understand. // check implementation
		//problem with scope resolution.. check
		std::cout<<"execfuncCall:: signature size: "<< i << std::endl;
		std::cout<<"execfuncCall:: assigned to variable: "<< funcExec->data.func.signatures->data.signatures.signature[i]->data.signature.assignment->data.assignment.name << std::endl;
		
		e->var[(funcExec->data.func.signatures->data.signatures.signature[i]->data.signature.assignment->data.assignment.name)] = eTmp->arr[(funcExec->data.func.signatures->data.signatures.signature.size())-(i+1)];
		std::cout<<"execfuncCall:: assigned: "<< e->var[(funcExec->data.func.signatures->data.signatures.signature[i]->data.signature.assignment->data.assignment.name)] << " to signature:: " << (funcExec->data.func.signatures->data.signatures.signature[i]->data.signature.assignment->data.assignment.name) << std::endl;
	}
	dispatchStatement(e,funcExec->data.func.statements);
	//after execution of function need to free env from function variable value
}

static void execPrint(struct ExecEnviron* e, struct AstElement* a)
{
	//logic for non func call, print in our case
	std::cout<<"execPrint:: param for the print statement: "<< a->data.call.param << std::endl;
	std::string name = a->data.call.param->data.array.element[0]->data.name;
	//std::cout<<dispatchArray(e, a->data.call.param)->arr<<std::endl; 
	// only pulling array is passed as param... so doesenot understand type vector or anything else
	ExecEnviron* eTmp = dispatchArray(e, a->data.call.param);
	if(e->isVector2d)
	{
		int i=0;
		char* vectorPos = vectorPoscalc(name,i);
		std::cout<<"name "<<name<<"vectorpos: "<<vectorPos<<std::endl;
		do
		{
			std::vector<int>arrTemp = e->vectorArr[(vectorPos)];
			std::copy(arrTemp.begin(), arrTemp.end(), std::ostream_iterator<int>(std::cout, " "));
			std::cout<<" "<<std::endl;
			i++;
			vectorPos = vectorPoscalc(name,i);
		}while( e->vectorArr.find(vectorPos) != e->vectorArr.end());
	}
	else
	{
		std::copy(eTmp->arr.begin(), eTmp->arr.end(), std::ostream_iterator<int>(std::cout, " "));
		std::cout<<" "<<std::endl;
	}
	
}

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
		}
    }
}

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



void execAst(struct ExecEnviron* e, struct AstElement* a)
{

        dispatchStatement(e, a);
	
}

struct ExecEnviron* createEnv()
{ 
	return new ExecEnviron();
}

void freeEnv(struct ExecEnviron* e)
{
    free(e);
}