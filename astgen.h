#include<vector>
#ifndef ASTGEN_H
#define ASTGEN_H

struct AstElement
{
	//this enum provides reference lookup for array search in astexec.cpp
    enum {ekId, ekNumber, ekBinExpression, ekAssignment, ekWhile, ekCall, ekStatements, ekIf, ekLastElement} kind;
    struct
    {
        int val;
        char* name; //only can accept char variable names not long names
        struct
        {
            struct AstElement *left, *right;
            char op;
        }expression;
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
    } data;
};

struct AstElement* makeAssignment(char*name, struct AstElement* val);
struct AstElement* makeExpByNum(int val);
struct AstElement* makeExpByName(char*name);
struct AstElement* makeExp(struct AstElement* left, struct AstElement* right, char op);
struct AstElement* makeStatement(struct AstElement* dest, struct AstElement* toAppend);
struct AstElement* makeWhile(struct AstElement* cond, struct AstElement* exec);
struct AstElement* makeCall(char* name, struct AstElement* param);
struct AstElement* makeIf(struct AstElement* cond, struct AstElement* ifTrue, struct AstElement* ifFalse);
#endif