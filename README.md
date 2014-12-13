![enter image description here](https://cdn3.iconfinder.com/data/icons/3d-printing-icon-set/256/Cube.png)
Welcome to BlueCompiler! 
===================


This is a barebones compiler written with the project [IFC Allignment](https://www.cms.bgu.tum.de/de/forschung/projekte/31-forschung/projekte/411-ifcalignment) in mind.  The interpreter was written in C++ for the project but it has scope to be plugged in with an LLVM implementation (refer developer documentation for details). A complete list of supported tokens and the supported inbuilt functions are provided below. A example program is also included to showcase what this language can do.

Feel free to extend and modify the source code as you please but please remember to provide references to this repo.

**

Logical Components and functions
----------------------------------------------

**

```sequence
Title: logical components
Lexer->Parser: scan text
Parser->Interpreter: read tokens, Create AST
Interpreter->Interpreter: execution environment,Run program
```


**

List of supported tokens and inbuilt functions
----------------------------------------------

**


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

The grammer in BNF form can be found in the developer documentation.
