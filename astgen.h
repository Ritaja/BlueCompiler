#include<vector>
#ifndef ASTGEN_H
#define ASTGEN_H

struct AstElement
{
    enum {ekId, ekNumber, ekBinExpression, ekAssignment, ekWhile, ekCall, ekStatements, ekLastElement} kind;
    struct
    {
        int val;
        char* name;
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
#endif