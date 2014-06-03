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

#endif