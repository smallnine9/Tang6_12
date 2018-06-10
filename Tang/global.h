#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 12
typedef enum
{	
	ENDFILE,ERROR,
	/*������*/
	INT,VOID,CHAR,MAIN,
	IF,ELSE,WHILE,RETURN,
	SCANF,PRINTF,CONST,FOR,

	/*�������*/
    PLUS,MINUS,MULTI,DIV,SEMI,LPAREN,
	RPAREN,LBRACKET,RBRACKET,GRE,GEQ,LSS,LEQ,
	NOT,NEQ,RBRACE,LBRACE,EQ,COMMA,
	/**/
	NUM,CCHAR,ID,SSTR
} TokenType;

typedef enum {constdeclareK, notvoidfunctionK, 
voidfunctionK, mainfunctionK, constdefineK, parameterK,
compstatementK, statementK, if_stmtK, while_stmtK, for_stmtK, return_stmtK,
useFunctionK, printf_stmtK, scanf_stmtK, factorK,
ConstK,intK,charK,statementlistK,expK,if_expK,OpK, 
val_parameterK,ArrayK,IdK,
TypeK,CompStmt} NodeKind;


typedef enum { Integer, Char } ExpType;

extern FILE* source;
extern FILE* listing;
extern FILE* code;
#define MAXCHILDREN 1

/*ʵ�����ڵ�Ľṹ*/

typedef struct treeNode
{
	struct treeNode * child[MAXCHILDREN];
	struct treeNode * sibling;/*����ʵ�����ı�׼��������ͬ���ṹ*/
	int lineno;
	NodeKind nodekind;
	union {
		TokenType op;
		int offset ;
		int val;
		char * name;
	} attr;
	ExpType type; /* for type checking of exps */
} TreeNode;

extern int EchoSource;

extern int lineno; //��Դ�ļ������к��õ�

extern int Error;

extern int TraceScan;
#endif