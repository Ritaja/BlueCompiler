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
    int x; /* The value of the x variable, a real language would have some name->value lookup table instead */
	std::map<char,int>var; //check why we need this
	std::map<std::string,AstElement*>func;//store the function with signature as key and store the functions
	std::map<char*,int>funvar;
};

static int execTermExpression(struct ExecEnviron* e, struct AstElement* a);
static int execBinExp(struct ExecEnviron* e, struct AstElement* a);
static void execAssign(struct ExecEnviron* e, struct AstElement* a);
static void execWhile(struct ExecEnviron* e, struct AstElement* a);
static void execCall(struct ExecEnviron* e, struct AstElement* a);
static void execStmt(struct ExecEnviron* e, struct AstElement* a);
static void execIf(struct ExecEnviron* e, struct AstElement* a);
static void execFunc(struct ExecEnviron* e, struct AstElement* a);
static int execSignatures(struct ExecEnviron* e, struct AstElement* a);
static int execSignature(struct ExecEnviron* e, struct AstElement* a);
static void execfuncCall(struct ExecEnviron* e, struct AstElement* a);
static void execPrint(struct ExecEnviron* e, struct AstElement* a);
static ExecEnviron* retFuncPtr(struct ExecEnviron* e, char* fname);

/* Lookup Array for AST elements which yields values */
static int(*valExecs[])(struct ExecEnviron* e, struct AstElement* a) =
{
    execTermExpression,
    execTermExpression,
    execBinExp,
    NULL,
    NULL,
    NULL,
    execSignatures,
    execSignature,
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
	execIf
};

/* Dispatches any value expression */
static int dispatchExpression(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(valExecs[a->kind]);
	std::cout<<"dispatchExpression:: Array Lookup no: "<< a->kind<<std::endl;
    return valExecs[a->kind](e, a);
}

static void dispatchStatement(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
	std::cout<< "Array Lookup no: "<<a->kind << runExecs[a->kind] << std::endl;
    assert(runExecs[a->kind]);
	std::cout<<"dispatchStatement:: "<<std::endl;
    runExecs[a->kind](e, a);
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

static int execTermExpression(struct ExecEnviron* e, struct AstElement* a)
{
    /* This function looks ugly because it handles two different kinds of
     * AstElement. I would refactor it to an execNameExp and execVal
     * function to get rid of this two if statements. */
    assert(a);
    if(AstElement::ekNumber == a->kind)
    {
        //return a->data.val; //return value of new variable after setting it
		//creates reference if not present else updates variable to new value
		//required to return the correct value for the correct variable
		//std::cout<<"Before violation"<<std::endl;
		//std::cout<<"execTermexp:: "<<(a->data.name)<<"and::"<<a->data.val<<"for ID: "<<&(a->data.name)<<std::endl;
		//e->var[*(a->data.name)] = a->data.val;
		std::cout<<"execTermexp:: value "<<a->data.val<<std::endl;
		return (a->data.val);
    }
    else
    {
        if(AstElement::ekId == a->kind)
        {
            onlyX(a->data.name); //only checks if new variable is defined
            assert(e);
            //return e->x; //returns if of type ID as value is assumed to be defined possible return x[*name]
			std::cout<<"execTermexp:: "<<e->var[*(a->data.name)]<<"ekID: "<<a->data.name<<std::endl;
			int temp = e->var[*(a->data.name)];
			return temp;
        }
    }
    fprintf(stderr, "OOPS: tried to get the value of a non-expression(%d)\n", a->kind);
    exit(1);
}

static int execBinExp(struct ExecEnviron* e, struct AstElement* a)
{
    assert(AstElement::ekBinExpression == a->kind);
    const int left = dispatchExpression(e, a->data.expression.left);
    const int right = dispatchExpression(e, a->data.expression.right);
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
	//check not all control paths return a value
}

static void execAssign(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(AstElement::ekAssignment == a->kind);
    onlyX(a->data.assignment.name);
    assert(e);
    struct AstElement* r = a->data.assignment.right;
    //e->x = dispatchExpression(e, r); //this needs to assign new value
	e->var[*(a->data.assignment.name)] = dispatchExpression(e, r); //check how this works
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

//added new
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

//has only print defined now
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
	for (int i=0;i<(funcExec->data.func.signatures->data.signatures.signature.size());i++) //check here for access violation
	{
		//most complicated line written. Take deep breath and break down in parts to understand. // check implementation
		//problem with scope resolution.. check
		std::cout<<"execfuncCall:: signature size: "<< i << std::endl;
		std::cout<<"execfuncCall:: assigned to variable: "<< funcExec->data.func.signatures->data.signatures.signature[i]->data.signature.assignment->data.assignment.name << std::endl;
		std::cout<<"execfuncCall:: param passed: "<< a->data.call.param << " Number: " << a->data.call.param->kind << std::endl;
		//e->var[*(funcExec->data.signatures.signature[i]->data.signature.assignment->data.assignment.name)] = dispatchExpression(e, a->data.call.param);
		e->var[*(funcExec->data.func.signatures->data.signatures.signature[i]->data.signature.assignment->data.assignment.name)] = dispatchExpression(e, a->data.call.param);
		std::cout<<"execfuncCall:: assigned: "<< e->var[*(funcExec->data.func.signatures->data.signatures.signature[i]->data.signature.assignment->data.assignment.name)] << " to signature:: " << *(funcExec->data.func.signatures->data.signatures.signature[i]->data.signature.assignment->data.assignment.name) << std::endl;
	}
	dispatchStatement(e,funcExec->data.func.statements);
	//after execution of function need to free env from function variable value
}

static void execPrint(struct ExecEnviron* e, struct AstElement* a)
{
	//logic for non func call, print in our case
	std::cout<<"execPrint:: param for the print statement: "<< a->data.call.param << std::endl;
    printf("%d\n", dispatchExpression(e, a->data.call.param)); //final aim is to return a value change here
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
        dispatchStatement(e, a->data.statements.statements[i]);
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
	retFuncPtr(e,fname);
	//dispatchStatement(e,a->data.func.signatures);

}

static ExecEnviron* retFuncPtr(struct ExecEnviron* e, char* fname)
{
	std::cout<<"the value of func pointer is: "<< e->func["funcA"] << std::endl;
	return e;
}

static int execSignatures(struct ExecEnviron* e, struct AstElement* a)
{
	std::cout<<"for the love of god I do reach here!"<<std::cout;
	return dispatchExpression(e,a->data.signatures.signature[0]);
}

static int execSignature(struct ExecEnviron* e, struct AstElement* a)
{
	std::cout<<"for the love of god I do reach here 2!"<<std::cout;
	return dispatchExpression(e,a->data.signature.assignment);
}

void execAst(struct ExecEnviron* e, struct AstElement* a)
{
    dispatchStatement(e, a);
}

struct ExecEnviron* createEnv()
{
    //assert(AstElement::ekLastElement == (sizeof(valExecs)/sizeof(*valExecs)));
    //assert(AstElement::ekLastElement == (sizeof(runExecs)/sizeof(*runExecs)));
    //return calloc(1, sizeof(struct ExecEnviron));
	return new ExecEnviron();
}

void freeEnv(struct ExecEnviron* e)
{
    free(e);
}