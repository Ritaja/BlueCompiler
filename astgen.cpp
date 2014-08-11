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

struct AstElement* makeExpByNum(int val)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekNumber;
    result->data.val = val;
	std::cout<<"makeExpByNum:: pushed val: "<<result->data.val<<std::endl;
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

struct AstElement* makeExp(struct AstElement* left, struct AstElement* right, char op)
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
	std::cout<<"makeStatement:: size of vector statements: "<<result->data.statements.statements.size()<<std::endl;
    result->data.statements.statements[result->data.statements.count-1] = toAppend;
	std::cout<<"makeStatement:: pushed: "<<(result->data.statements.statements[result->data.statements.count-1])<<std::endl;
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

struct AstElement* makeCall(char* name, struct AstElement* param)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekCall;
    result->data.call.name = name;
    result->data.call.param = param;
	std::cout<<"makeCall:: pushed: "<<std::endl;
	std::cout<<"makeCall:: param: "<< result->data.call.param <<std::endl;
    return result;
}

struct AstElement* makeFunc(char* name, struct AstElement* signature, struct AstElement* statement)
{
	struct AstElement* result = new AstElement();
	result->kind = AstElement::ekFunc;
	result->data.func.name = name;
	result->data.func.signatures = signature;
	result->data.func.statements= statement;
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