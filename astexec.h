#include <map>
#ifndef ASTEXEC_H
#define ASTEXEC_H

struct AstElement;
struct ExecEnviron;

/* creates the execution engine */
struct ExecEnviron* createEnv();

/* removes the ExecEnviron */
void freeEnv(struct ExecEnviron* e);

/* executes an AST */
void execAst(struct ExecEnviron* e, struct AstElement* a);

/*Sets the propertySet for the created environment by the name and double value.*/
void setPropertySet(struct ExecEnviron* e,char* varName, double val);


/*returns the value set for a particular propertySet element in the exec env.*/
double getPropertySet(struct ExecEnviron* e, char* varName);


/*returns the entire map of the propertySet element in the exec env.*/
std::map<std::string,double> getPropertySet(struct ExecEnviron* e);

/*Sets the propertySet for the created environment by the name and double value. For vector2d type.*/
void setPropertySet(struct ExecEnviron* e,char* varName, double val0, double val1);

#endif