#include <vector>
#include <iostream>
#include "astgen.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



static void* checkAlloc(size_t sz)
{
    void* result = calloc(sz, 1);
	if(!result)
    {
        fprintf(stderr, "alloc failed\n");
        exit(1);
    }
}

struct AstElement* makeAssignment( char*name, struct AstElement* val)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekAssignment;
    result->data.assignment.name = name;
    result->data.assignment.right = val;
	std::cout<<"makeAssign:: pushed: "<<std::endl;
	std::cout<<"makeAssign:: pushed var: "<<result->data.assignment.name<<std::endl;
	std::cout<<"makeAssign:: pushed val: "<<result->data.assignment.right<<std::endl;
    return result;
}

struct AstElement* makeFuncAssignment( char*name, struct AstElement* val)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekFuncAssign;
    result->data.assignment.name = name;
    result->data.assignment.right = val;
	std::cout<<"makeFuncAssign:: pushed: "<<std::endl;
	std::cout<<"makeFuncAssign:: pushed var: "<<result->data.assignment.name<<std::endl;
	std::cout<<"makeFuncAssign:: pushed val: "<<result->data.assignment.right<<std::endl;
    return result;
}

//used as function parameter (value-passing) initialised with null later on value would be assigned
struct AstElement* makeAssignment( char*name)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekAssignment;
    result->data.assignment.name = name;
	result->data.assignment.right = NULL;
	std::cout<<"makeAssign (for signature):: pushed: "<<std::endl;
	std::cout<<"makeAssign (Si):: pushed var: "<<result->data.assignment.name<<std::endl;
	std::cout<<"makeAssign (Si):: pushed val: "<<result->data.assignment.right<<std::endl;
    return result;
}

struct AstElement* makeExpByNum(double val)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekNumber;
    result->data.val = val;
	std::cout<<"makeExpByNum:: pushed val: "<<val<<std::endl;
    return result;
}


struct AstElement* makeArray(struct AstElement* result, struct AstElement* expression)
{
	
	if(!result)
    {
	      result = new AstElement();
		  result->kind = AstElement::ekArray;
		  result->data.array.count=0;
	}
	assert(AstElement::ekArray == result->kind);
	result->data.array.count++;
	result->data.array.element.resize(result->data.array.count);
	result->data.array.element[result->data.array.count-1]=expression;
	std::cout<<"makeArray:: count"<<result->data.array.count<<" Pushed:: "<<result->data.array.element[result->data.array.count-1]->kind<<std::endl;
	return result;
}

struct AstElement* makeVector(char* name, struct AstElement* vector)
{
	std::cout<<"MakeVector:: name:: "<< name <<std::endl;
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekVector;
	std::cout<<"MakeVector:: kind:: "<<result->kind<<std::endl;
	result->data.vector.name = name;
	result->data.vector.array = vector;
	return result;
}

struct AstElement* makeNullVector(char* name)
{
	std::cout<<"MakeNullVector:: "<<std::endl;
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekVector;
	result->data.vector.name = name;
	result->data.vector.array = NULL;
	return result;
}


struct AstElement* makeVector2d(char* name, struct AstElement* vectors)
{
	std::cout<<"MakeVector2d:: "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekVector2d;
	result->data.vector2d.name=name;
	result->data.vector2d.vectors=vectors;
	return result;
}

struct AstElement* makeNullVectors()
{
	struct AstElement* result = new AstElement();
	result->data.vectors.count=0;
	result->data.vectors.vector.resize(1);
	result->data.vectors.vector[result->data.vectors.count]=NULL;
	result->data.vectors.name = NULL;
	std::cout<<"MakeNullVectors:: "<<std::endl;
	return result;
}

struct AstElement* makeVectors(struct AstElement* result, struct AstElement* array)
{
	if(!result)
    {
	      result = new AstElement();
		  result->kind = AstElement::ekVectors;
		  result->data.vectors.count=0;
	}
	assert(AstElement::ekVectors == result->kind);
	result->data.vectors.count++;
	result->data.vectors.vector.resize(result->data.vectors.count);
	result->data.vectors.vector[result->data.vectors.count-1]=array;
	result->data.vectors.name = NULL;
	std::cout<<"MakeVectors:: "<<std::endl;
	return result;
}

struct AstElement* makeExpByName(char*name)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekId; //check here for value of name then assign maybe
    result->data.name = name;
    return result;
}



struct AstElement* makeExp(struct AstElement* left, struct AstElement* right, char* op)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekBinExpression;
    result->data.expression.left = left;
    result->data.expression.right = right;
    result->data.expression.op = op;
	std::cout<<"SCOPE ASTGEN::operator found at: "<<left<<op<<right<<std::endl;
    return result;
}

struct AstElement* makeStatement(struct AstElement* result, struct AstElement* toAppend)
{
	//first statement read
    if(!result)
    {
        //result = checkAlloc(sizeof(*result));
		result = new AstElement();
        result->kind = AstElement::ekStatements;
        result->data.statements.count = 0;
		std::cout<<"makeStatement:: count: "<<result->data.statements.count<<std::endl;
        //result->data.statements.statements = 0;
		//std::vector <AstElement**> x;
    }
    assert(AstElement::ekStatements == result->kind);
    result->data.statements.count++;
	//increases the size of the pointer by the required no using realloc:: obsolete in C++
    //result->data.statements.statements = realloc(result->data.statements.statements, result->data.statements.count*sizeof(*result->data.statements.statements));
	result->data.statements.statements.resize(result->data.statements.count);
	std::cout<<"makeStatement:: no of Statement: "<<result->data.statements.count<<std::endl;
	std::cout<<"makeStatement:: kind: "<<result->kind<<std::endl;
    result->data.statements.statements[result->data.statements.count-1] = toAppend;
	std::cout<<"makeStatement:: pushed: "<<(result->data.statements.statements[result->data.statements.count-1])->kind<<std::endl;
    return result;
}

struct AstElement* makeWhile(struct AstElement* cond, struct AstElement* exec)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekWhile;
    result->data.whileStmt.cond = cond;
    result->data.whileStmt.statements = exec;
	std::cout<<"makeWhile:: pushed: "<<std::endl;
    return result;
}

struct AstElement* makeIf(struct AstElement* cond, struct AstElement* ifTrue, struct AstElement* ifFalse)
{
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekIf;
	result->data.ifStatement.cond = cond;
	result->data.ifStatement.ifTrue = ifTrue;
	result->data.ifStatement.ifFalse = ifFalse;
	std::cout<<"makeIf:: pushed: "<<std::endl;
	return result;
}

struct AstElement* makeIf(struct AstElement* cond, struct AstElement* ifTrue)
{
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekIf;
	result->data.ifStatement.cond = cond;
	result->data.ifStatement.ifTrue = ifTrue;
	result->data.ifStatement.ifFalse = NULL;
	std::cout<<"makeIf:: pushed: "<<std::endl;
	return result;
}

struct AstElement* makeElseIf(struct AstElement* condIf, struct AstElement* ifTrue, struct AstElement* condElseIf, struct AstElement* elseIfTrue, struct AstElement* elseIfFalse)
{
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekElseIf;
	result->data.elseifStatement.ifcond = condIf;
	result->data.elseifStatement.ifTrue = ifTrue;
	result->data.elseifStatement.elseifCond = condElseIf;
	result->data.elseifStatement.elseifCondTrue = elseIfTrue;
	result->data.elseifStatement.elseifCondFalse = elseIfFalse;
	return result;
}

struct AstElement* makeCall(char* name, struct AstElement* param)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekCall;
    result->data.call.name = name;
	result->data.call.param = param;
	std::cout<<"makeCall:: pushed: "<<result->data.call.name<<std::endl;
	std::cout<<"makeCall:: param: "<< result->data.call.param <<std::endl;
    return result;
}

struct AstElement* makeFunc(char* name, struct AstElement* signature, struct AstElement* statement)
{
	std::cout<<"\n ########         makeFunction:: creating "<<name<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekFunc;
	result->data.func.name = name;
	result->data.func.signatures = signature;
	result->data.func.statements= statement;
	std::cout<<"makeFunc:: pushed: "<<std::endl;
	return result;
}

struct AstElement* makeFunc(char* name, struct AstElement* statement)
{
	std::cout<<"\n ########         makeFunction:: creating "<<name<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekFunc;
	result->data.func.name = name;
	result->data.func.signatures = NULL;
	result->data.func.statements= statement;
	result->data.func.count=0;
	std::cout<<"makeFunc:: pushed: "<<std::endl;
	return result;
}

//check implementation
struct AstElement* makeSignatures(struct AstElement* result,struct AstElement* signature)
{
	if(!result)
	{
		std::cout<<"\n makeSignatures:: creating result";
        result = new AstElement();
		result->kind = AstElement::ekSignatures;
		result->data.signatures.count = 0;
	}
	result->data.signatures.count++;
	result->data.signatures.signature.resize(result->data.signatures.count);
	result->data.signatures.signature[result->data.signatures.count-1] = signature;
	//test code
	std::cout<<"\n Signature: pushed signature: "<<result->data.signatures.signature[result->data.signatures.count-1];	
	return result;
}


struct AstElement* makeSignatures()
{
	//blank signature with count 0
	std::cout<<"\n makeSignatures:: creating result";
    AstElement* result = new AstElement();
	result->kind = AstElement::ekSignatures;
	result->data.signatures.count = 0;
	result->data.signatures.signature[0] = NULL;
		
	return result;
}


struct AstElement* makeSignature(char* type,struct AstElement* assignment)
{
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekSignature;
	result->data.signature.type = type;
	std::cout<<"\n makeSignature:pushed type: "<<result->data.signature.type;
	result->data.signature.assignment = assignment;
	std::cout<<"\n makeSignature:pushed assignment: "<<result->data.signature.assignment;
	return result;
}

/* cretes a vector1d element for element lookup  */
struct AstElement* makeVec1delement(char* name,int elementPos)
{
	struct AstElement* result = new AstElement();
	result->data.Vec1delement.name = name;
	result->data.Vec1delement.elementPos = elementPos;
	result->kind = AstElement::ekVec1delement;
	return result;
}

/* cretes a vector2d element for element lookup  */
struct AstElement* makeVec2delement(char* name, int elementPos1, int elementPos2)
{
	struct AstElement* result = new AstElement();
	result->data.Vec1delement.name = name;
	result->data.Vec1delement.elementPos = elementPos1;
	result->data.Vec1delement.elementPos = elementPos2;
	result->kind = AstElement::ekVec2delement;
	return result;
}

/* cretes a structure for vector2d assignment to work with individual elements  */
struct AstElement* makeVecAssignment(char* name, int elementPos, struct AstElement* right)
{
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekVecAssignment;
	result->data.VecAssignment.name = name;
	result->data.VecAssignment.elementPos1 = elementPos;
	result->data.VecAssignment.right = right;
	return result;
}

/* cretes a structure for vector2d assignment to work with individual elements  */
struct AstElement* makeVec2dAssignment(char* name, int elementPos1, int elementPos2, struct AstElement* right)
{
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekVec2dAssignment;
	result->data.Vec2dAssignment.name = name;
	result->data.Vec2dAssignment.elementPos1 = elementPos1;
	result->data.Vec2dAssignment.elementPos2 = elementPos2;
	result->data.Vec2dAssignment.right = right;
	return result;
}

struct AstElement* makeReturnByExp(struct AstElement* exp)
{
	std::cout<<"\n makeReturnByExp:pushed type: "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekRtrnByExp;
	result->data.returnData.exp = exp;
	return result;
}


struct AstElement* makePow( struct AstElement* left, struct AstElement* right)
{
	std::cout<<"\n makePow "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekPow;
	result->data.pow.right = right;
	result->data.pow.left = left;
	return result;
}

struct AstElement* makeMin( struct AstElement* first, struct AstElement* second,struct AstElement* third)
{
	std::cout<<"\n makeMin "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekMin;
	result->data.min.first = first;
	result->data.min.second = second;
	result->data.min.third = third;
	return result;
}

struct AstElement* makeFact( struct AstElement* expr)
{
	std::cout<<"\n makefact "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekFact;
	result->data.fact.expr = expr;
	return result;
}

struct AstElement* makeAcos( struct AstElement* expr)
{
	std::cout<<"\n makeAcos "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekAcos;
	result->data.Acos.expr = expr;
	return result;
}

struct AstElement* makeSqrt( struct AstElement* expr)
{
	std::cout<<"\n makeSqrt "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekSqrt;
	result->data.Sqrt.expr = expr;
	return result;
}

struct AstElement* makeRotatez( struct AstElement* expr)
{
	std::cout<<"\n makeRotatez "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekRotatez;
	result->data.rotatez.expr = expr;
	return result;
}

struct AstElement* makeMagnitudesqr( struct AstElement* expr)
{
	std::cout<<"\n makeMagnitudesqr "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekMagnitudesqr;
	result->data.magnitudeSqr.expr = expr;
	return result;
}

struct AstElement* makeTransform( struct AstElement* expr)
{
	std::cout<<"\n makeTransform "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekTransform;
	result->data.transform.expr = expr;
	return result;
}

struct AstElement* makeDot( struct AstElement* left,struct AstElement* right)
{
	std::cout<<"\n makeDot "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekDot;
	result->data.dot.left = left;
	result->data.dot.right = right;
	return result;
}

struct AstElement* makeCross( struct AstElement* first,struct AstElement* second)
{
	std::cout<<"\n makeCross "<<std::endl;
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekCross;
	result->data.cross.first = first;
	result->data.cross.second = second;
	return result;
}