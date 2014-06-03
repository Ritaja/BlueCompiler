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
    return result;
}

struct AstElement* makeExpByNum(int val)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekNumber;
    result->data.val = val;
    return result;
}

struct AstElement* makeExpByName(char*name)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekId;
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
		std::cout<<"count: "<<result->data.statements.count<<std::endl;
        //result->data.statements.statements = 0;
		//std::vector <AstElement**> x;
    }
    assert(AstElement::ekStatements == result->kind);
    result->data.statements.count++;
	std::cout<<"size of vector: "<<result->data.statements.statements.size()<<std::endl;
	//increases the size of the pointer by the required no using realloc:: obsolete in C++
    //result->data.statements.statements = realloc(result->data.statements.statements, result->data.statements.count*sizeof(*result->data.statements.statements));
	result->data.statements.statements.resize(result->data.statements.count);
    result->data.statements.statements[result->data.statements.count-1] = toAppend;
	std::cout<<"pushed: "<<result->data.statements.statements[result->data.statements.count-1]<<std::endl;
    return result;
}

struct AstElement* makeWhile(struct AstElement* cond, struct AstElement* exec)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekWhile;
    result->data.whileStmt.cond = cond;
    result->data.whileStmt.statements = exec;
    return result;
}

struct AstElement* makeCall(char* name, struct AstElement* param)
{
    //struct AstElement* result = checkAlloc(sizeof(*result));
	struct AstElement* result = new AstElement();
    result->kind = AstElement::ekCall;
    result->data.call.name = name;
    result->data.call.param = param;
    return result;
}