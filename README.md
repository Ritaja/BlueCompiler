![enter image description here](https://cdn3.iconfinder.com/data/icons/3d-printing-icon-set/256/Cube.png)
Welcome to BlueCompiler! 
===================
![enter image description here](https://lh5.googleusercontent.com/DXaAlI3SBkadm9BNQMgcCPtTcgTGLZ3gZhRAytdIdw=s0)

This is a barebones compiler written with the project [IFC Allignment](https://www.cms.bgu.tum.de/de/forschung/projekte/31-forschung/projekte/411-ifcalignment) in mind.  The interpreter was written in C++ for the project but it has scope to be plugged in with an LLVM implementation (refer developer documentation for details). A complete list of supported tokens and the supported inbuilt functions are provided below. A example program is also included to showcase what this language can do.

Feel free to extend and modify the source code as you please but please remember to provide references to this repo.



### Table of contents

You can insert a table of contents using the marker `[TOC]`:

[TOC]


Logical components
----------------------------------------------



![enter image description here](https://lh3.googleusercontent.com/-0ezxCgqcDG8/VI2jJbJojcI/AAAAAAAABrU/Jk8Jt0CUZd8/s0/Screen+Shot+2014-12-14+at+15.42.18+1.png "Compiler Logical Components")



Language Layout
----------------------------------------------

The basic programming language that the compiler understands is organised in blocks. An overview of the language that the compiler understands:


![enter image description here](https://lh4.googleusercontent.com/-H8XpEukV78A/VI25O6bzXMI/AAAAAAAABr0/9SCisNK9Z_4/s0/Screen+Shot+2014-12-14+at+16.10.59.png "Program structure Example")


Railroad Diagrams for the Grammar
----------------------------------------------
The grammar is the star of the show here, so if things start getting confusing please revert to the full blown html which can be found in the repo as Grammar.html

![enter image description here](https://lh3.googleusercontent.com/-TuKrJkAMc1g/VI3JejsrYbI/AAAAAAAABsI/5hqzJBVIVJU/s0/array.png "array.png")

![enter image description here](https://lh5.googleusercontent.com/-ZdqNqg_ZffA/VI3Jw75sWpI/AAAAAAAABsU/mnpKxpkMhHE/s0/arrExpression.png "arrExpression.png")

![enter image description here](https://lh6.googleusercontent.com/-z6pPnujtJDA/VI3J3WKDlWI/AAAAAAAABsg/p2iMh9ikmIc/s0/Assignment.png "Assignment.png")

![enter image description here](https://lh3.googleusercontent.com/-ERgXPQNBMig/VI3J-Gq55GI/AAAAAAAABss/tIjp-61N728/s0/Block.png "Block.png")

![enter image description here](https://lh6.googleusercontent.com/-LszOKb29oBw/VI3KEJwgyxI/AAAAAAAABs4/QGrT70OWxiE/s0/call.png "call.png")

![enter image description here](https://lh4.googleusercontent.com/-eSKVTHq-16o/VI3KJq4Qe8I/AAAAAAAABtE/sed2iq7nnjk/s0/Expression.png "Expression.png")

![enter image description here](https://lh6.googleusercontent.com/-wYe7m6CuACU/VI3KdR9_vyI/AAAAAAAABtQ/ygzKg1VtzYw/s0/func.png "func.png")

![enter image description here](https://lh4.googleusercontent.com/-YT0Clc4mlxA/VI3KjEJCP8I/AAAAAAAABtc/8bvsOb8mXBM/s0/ifStmt.png "ifStmt.png")

![enter image description here](https://lh4.googleusercontent.com/-c6VqKPuXWps/VI3KodoNzqI/AAAAAAAABto/PimihaYzUoU/s0/Program.png "Program.png")

![enter image description here](https://lh4.googleusercontent.com/-DIX3-HNow_M/VI3KtZtVwkI/AAAAAAAABt0/2tvho8L8nAs/s0/Return.png "Return.png")

![enter image description here](https://lh6.googleusercontent.com/-08aqTV98xr4/VI3KycqLMWI/AAAAAAAABuE/3dAMVm_7iGM/s0/Signature.png "Signature.png")

![enter image description here](https://lh4.googleusercontent.com/-JZyoxf4KpPI/VI3K5aNR4lI/AAAAAAAABuQ/hextNUY-KsI/s0/Signatures.png "Signatures.png")

![enter image description here](https://lh5.googleusercontent.com/-XFfSVnlKOec/VI3K_pxJeMI/AAAAAAAABuc/shGusq9e-wI/s0/statement.png "statement.png")

![enter image description here](https://lh6.googleusercontent.com/-slP5ZeGY_Ps/VI3LEz8RnqI/AAAAAAAABus/110quBbAYjU/s0/Statements.png "Statements.png")

![enter image description here](https://lh6.googleusercontent.com/-GX4UGTtnyGc/VI3LJowuDvI/AAAAAAAABvE/NbG9_l3xURs/s0/vector2d.png "vector2d.png")

![enter image description here](https://lh3.googleusercontent.com/-X9R3mEqp2C8/VI3LOHj5kCI/AAAAAAAABvQ/vYa44hQmuZE/s0/vectors.png "vectors.png")

![enter image description here](https://lh6.googleusercontent.com/-t7rphKvCWYg/VI3LT7ChMtI/AAAAAAAABvc/SNu94oq-Orc/s0/whileStmt.png "whileStmt.png")

List of supported tokens and inbuilt functions
----------------------------------------------



| Token(in Program)    | Internal Representation |
| :------- | ----: |
| while | TOKEN_WHILE |
| != , <= , >= , && , || , ==   | TOKEN_OPERATOR |
| ￼￼{     | TOKEN_BEGIN   | 
| ￼￼}     | TOKEN_END   |
| ￼￼[    | BOX_OPEN |
| ￼￼]    | BOX_CLOSE |
| ￼￼func    | BEGIN_FUNC   |
| ￼￼do    | TOKEN_DO   |
| ￼￼if     | TOKEN_IF   |
| ￼￼else     | TOKEN_ELSE   |
| ￼￼vector     | TOKEN_VECTOR   |
| ￼￼vector2d     | TOKEN_VECTOR2d   |
| ￼￼return     | TOKEN_RETURN  |
| ￼￼pow     | TOKEN_POW   |
| acos    | TOKEN_ACOS|
| ￼￼sqrt   | TOKEN_SQRT  |
| ￼￼rotatez   | TOKEN_ROTATEZ |
| ￼￼magnitude_squared    | TOKEN_MAGNITUDESQR  |
| ￼￼transform   | TOKEN_TRANSFORM  |
| min   | TOKEN_MIN  |
| dot   | TOKEN_DOT |
| cross  | TOKEN_CROSS |
| [a-zA-Z_][a-zA-Z0-9_]*  | TOKEN_ID |
| [-]?[0-9]*[.]?[0-9]+  | TOKEN_NUMBER  |
| * , /, +, -, <, >, % | TOKEN_OPERATOR  |

The grammar in BNF form can be found in the developer documentation.




