#include<vector>
#include<string>
#ifndef ASTGEN_H
#define ASTGEN_H

struct AstElement
{
	//this enum provides reference lookup for array search in astexec.cpp
    enum {ekId, ekNumber, ekBinExpression, ekAssignment, ekWhile, ekFunc, ekSignatures, ekSignature, ekCall, ekStatements, ekIf, ekArray, ekLastElement} kind;
    struct
    {
        int val; //only one value... look for arrays implementation
        char* name; 
        struct
        {
            struct AstElement *left, *right;
            char op;
        }expression;
		//create a left node here to hold data type as int or double
        struct
        {
            char*name;
            struct AstElement* right;
        }assignment;
        struct
        {
            int count;
            //struct AstElement** statements;
			std::vector<struct AstElement*>statements;
        }statements;
        struct
        {
            struct AstElement* cond;
            struct AstElement* statements;
        } whileStmt;
		struct 
		{
			struct AstElement* cond;
			struct AstElement* ifTrue;
			struct AstElement* ifFalse;
		}ifStatement;
        struct
        {
            char* name;
            struct AstElement* param;
        }call;
		//added function what is call above
		struct
		{
			char* name;
			struct AstElement* signatures;
			struct AstElement* statements;
		}func;
		struct
		{
			int count;
			std::vector<struct AstElement*>signature;
		}signatures;
		//represent one signature e.g. int ab
		struct
		{
			char* type;
			struct AstElement* assignment;
		}signature;
		struct
		{
			int count;
			std::vector<struct AstElement *>element;
		}array;
    } data;
};

struct AstElement* makeAssignment(char*name, struct AstElement* val);
struct AstElement* makeAssignment(char*name);
struct AstElement* makeExpByNum(int val);
struct AstElement* makeExpByName(char*name);
struct AstElement* makeExp(struct AstElement* left, struct AstElement* right, char op);
struct AstElement* makeStatement(struct AstElement* dest, struct AstElement* toAppend);
struct AstElement* makeWhile(struct AstElement* cond, struct AstElement* exec);
struct AstElement* makeCall(char* name, struct AstElement* param);
struct AstElement* makeIf(struct AstElement* cond, struct AstElement* ifTrue, struct AstElement* ifFalse);
struct AstElement* makeFunc(char* name, struct AstElement* signature, struct AstElement* statement);
struct AstElement* makeSignatures(struct AstElement* result,struct AstElement* signature);
struct AstElement* makeSignature(char* type,struct AstElement* assignment);
struct AstElement* makeArray(struct AstElement* result, struct AstElement* expression);
struct AstElement* makeArray2d(struct AstElement* result, struct AstElement* expression);
#endif