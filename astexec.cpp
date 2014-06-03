#include "astexec.h"
#include "astgen.h"
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct ExecEnviron
{
    int x; /* The value of the x variable, a real language would have some name->value lookup table instead */
};

static int execTermExpression(struct ExecEnviron* e, struct AstElement* a);
static int execBinExp(struct ExecEnviron* e, struct AstElement* a);
static void execAssign(struct ExecEnviron* e, struct AstElement* a);
static void execWhile(struct ExecEnviron* e, struct AstElement* a);
static void execCall(struct ExecEnviron* e, struct AstElement* a);
static void execStmt(struct ExecEnviron* e, struct AstElement* a);

/* Lookup Array for AST elements which yields values */
static int(*valExecs[])(struct ExecEnviron* e, struct AstElement* a) =
{
    execTermExpression,
    execTermExpression,
    execBinExp,
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
    execCall,
    execStmt,
};

/* Dispatches any value expression */
static int dispatchExpression(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(valExecs[a->kind]);
    return valExecs[a->kind](e, a);
}

static void dispatchStatement(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(runExecs[a->kind]);
    runExecs[a->kind](e, a);
}

static void onlyName(const char* name, const char* reference, const char* kind)
{
    if(strcmp(reference, name))
    {
        fprintf(stderr,
            "This language knows only the %s '%s', not '%s'\n",
            kind, reference, name);
        exit(1);
    }
}

static void onlyX(const char* name)
{
    onlyName(name, "x", "variable");
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
        return a->data.val;
    }
    else
    {
        if(AstElement::ekId == a->kind)
        {
            onlyX(a->data.name);
            assert(e);
            return e->x;
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
        default:
            fprintf(stderr,  "OOPS: Unknown operator:%c\n", a->data.expression.op);
            exit(1);
    }
    /* no return here, since every switch case returns some value (or bails out) */
}

static void execAssign(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(AstElement::ekAssignment == a->kind);
    onlyX(a->data.assignment.name);
    assert(e);
    struct AstElement* r = a->data.assignment.right;
    e->x = dispatchExpression(e, r);
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

static void execCall(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(AstElement::ekCall == a->kind);
    onlyPrint(a->data.call.name);
    printf("%d\n", dispatchExpression(e, a->data.call.param));
}

static void execStmt(struct ExecEnviron* e, struct AstElement* a)
{
    assert(a);
    assert(AstElement::ekStatements == a->kind);
    int i;
	//traversing of the aray of structures for each statement now its vectors
	for(i=0; i<a->data.statements.statements.size(); i++)
    {
		std::cout<<"size"<<a->data.statements.statements.size()<<std::endl;
        dispatchStatement(e, a->data.statements.statements[i]);
    }
}

void execAst(struct ExecEnviron* e, struct AstElement* a)
{
    dispatchStatement(e, a);
}

struct ExecEnviron* createEnv()
{
    assert(AstElement::ekLastElement == (sizeof(valExecs)/sizeof(*valExecs)));
    assert(AstElement::ekLastElement == (sizeof(runExecs)/sizeof(*runExecs)));
    //return calloc(1, sizeof(struct ExecEnviron));
	return new ExecEnviron();
}

void freeEnv(struct ExecEnviron* e)
{
    free(e);
}