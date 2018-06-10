#include "global.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

char *id_reserve;
int ParseBack = FALSE;

static TokenType token;

static TreeNode * program(void);
static TreeNode * constdeclare(void);
//static TreeNode * variabledeclare(void);
static TreeNode * notvoidfunction(void);
static TreeNode * voidfunction(void);
static TreeNode * mainfunction(void);
static TreeNode * constdefine(void);
//static TreeNode * typeid(void);
static TreeNode * parameter(void);
static TreeNode * compstatement(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * while_stmt(void);
static TreeNode * for_stmt(void);
static TreeNode * useFunction(void);
static TreeNode * val_parameter(void);
static TreeNode * return_stmt(void);
static TreeNode * scanf_stmt(void);
static TreeNode * printf_stmt(void);
static TreeNode * exp(void);
static TreeNode * factor(void);
static TreeNode * statementlist(void);
static TreeNode * if_exp(void);
static TreeNode * assign_stmt(void);

static void syntaxError(char * message)
{
	fprintf(listing, "\n>>> ");
	fprintf(listing, "Syntax error at line %d: %s", lineno, message);
	Error = TRUE;
}
static void match(TokenType expected)
{
	if (token == expected)
	{
		token = getToken();
	//	printToken(token, tokenString);
	}
	else
	{
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		fprintf(listing, "      ");
	}
}

/*节点函数是用来创建节点，并且指明节点的子节点和子节点的兄弟节点*/
/*＜程序＞ ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞*/
TreeNode * program() /*我省略了program节点，节省一点空间*/
{
	TreeNode *t = NULL;
	TreeNode *p = NULL;
	switch (token)  //读取第一个字符
	{
	case CONST:
		t = constdeclare();
		break;
	case INT:  
		t = notvoidfunction();
		break;
	case CHAR:
		t = notvoidfunction();
		break;
	case VOID:
	{
		match(VOID);  /*警告：这里已经match掉了VOID*/
	   if (token == MAIN)
		t = mainfunction();
	    else if (token == ID)
		t = voidfunction();
	    else syntaxError("voidf or main write  error");
	    break; 
	}
	default:
		syntaxError("first token program error!");
		return t;
	}
	p = t;
	/*非主函数可以有多个*/
	if (t == NULL)
	{
		syntaxError("节点为空");
		return t;
	}
	while (token != ENDFILE)
	{
		if (token == INT || token == CHAR)
		{
			p->sibling = notvoidfunction();
			p = p->sibling;
		}
		else if (token == VOID)
		{
			match(VOID);
			if (token == MAIN)
			{
				p->sibling = mainfunction();
				p = p->sibling;
			}
			else if (token == ID)
			{
				p->sibling = voidfunction();
				p = p->sibling;
			}
			else
			{
				syntaxError("voidf or main not find  error");
				return t;
			}
		}
		else
		{ 
		 syntaxError("program error");
		 return t;
		}
	}
	return t;
}
TreeNode *constdeclare()
{
	match(CONST);
	TreeNode *t = makeNode(constdeclareK);
	TreeNode *p = t;
	t->child[0] = constdefine();
	match(SEMI);
	while (token == CONST)
	{
		p->child[0]->sibling = constdefine();
		p = p->child[0]->sibling;
		match(SEMI);
	}
	return t;
}

TreeNode * notvoidfunction()
{
	TreeNode *t = makeNode(notvoidfunctionK);
	TreeNode *p = t;
	if (token == INT)
	{
		match(INT);
		t->type = Integer;
	}
	else if (token == CHAR)
	{
		match(CHAR);
		t->type = Char;
	}
	t->attr.name = copyString(tokenString);
	match(ID);
	match(LPAREN);
	p->child[0] = parameter();
	match(RPAREN);
	match(LBRACE);
	p->child[0]->sibling = compstatement();
	match(RBRACE);
	return t;

}
TreeNode * voidfunction()
{
	TreeNode *t = makeNode(voidfunctionK);
	TreeNode *p = t;
	t->attr.name = copyString(tokenString);
	match(LPAREN);
	p->child[0] = parameter();
	match(RPAREN);
	match(LBRACE);
	p->child[0]->sibling = compstatement();
	match(RBRACE);
	return t;
}
TreeNode * mainfunction()
{
	TreeNode * t = makeNode(mainfunctionK);
	match(MAIN);
	match(LPAREN);
	match(RPAREN);
	match(LBRACE);
	t->child[0] = compstatement();
	match(RBRACE);
	return t;
}
TreeNode * constdefine()
{
	TreeNode *t = makeNode(constdefineK);
	TreeNode *p = t;
	if (token == INT)
	{
		match(INT);
		t->type = Integer;
	}
	else if (token == CHAR)
	{
		match(CHAR);
		t->type = Char;
	}
	p->child[0] = makeNode(ConstK);
	p->child[0]->attr.name = copyString(tokenString);
	match(ID);
	match(EQ);
	p->child[0]->attr.val = atoi(tokenString);//给这个常量节点赋常量名和常量值
		while (token == COMMA)
		{
			match(COMMA);
            
			p->child[0]->sibling = makeNode(ConstK);
			p = p->child[0]->sibling;
			p->attr.name = copyString(tokenString);
			match(ID);
			match(EQ);

			p->attr.val = atoi(tokenString);

		}
		return t;
}
/*＜参数表＞ ::= ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞*/
TreeNode* parameter()
{
	TreeNode *t = makeNode(parameterK);
	TreeNode *p = t;
	if (token == INT)
	{
		match(INT);
		t ->child[0]= makeNode(intK);//生成一个整数节点	
		t->child[0]->attr.name = copyString(tokenString);
		t->child[0]->attr.val = atoi(tokenString);
		match(ID);
	}
	else if (token == CHAR)
	{
		match(CHAR);
		t->child[0] = makeNode(charK);//生成一个字符节点	
		t->child[0]->attr.name = copyString(tokenString);
		match(ID);
	}
	while (token == COMMA)
	{
		match(COMMA);
		if (token == INT)
		{
			match(INT);
			t->child[0] = makeNode(intK);//生成一个整数节点	
			t->child[0]->attr.name = copyString(tokenString);
			t->child[0]->attr.val = atoi(tokenString);
		}
		else if (token == CHAR)
		{
			match(CHAR);
			t->child[0] = makeNode(charK);//生成一个字符节点	
			t->child[0]->attr.name = copyString(tokenString);
		}
	}
	if (t->child[0] == NULL)
	{
		t = NULL;
	}
	return t;

}
TreeNode* compstatement()
{
	TreeNode *t =makeNode(compstatementK);
	TreeNode *p = t;
	if (token == CONST)
	{
		t->child[0] = constdeclare();
		
	}
	if (token == IF || token == WHILE || token == FOR ||
		token == LBRACE || token == ID || token == RETURN || token == SCANF || token == PRINTF)
	{
		if (t->child[0] != NULL)
			t->child[0]->sibling = statementlist();
		else
			t->child[0] = statementlist();
		  //t = statementlist() ,好像可以替换掉上面这个语句，简化树
	}
	else
	{
		syntaxError("复合语句书写错误");
	}
	return t;
}
TreeNode * statementlist()
{
	TreeNode* t = makeNode(statementlistK);

	TreeNode* p = t;
	if (token == IF || token == WHILE || token == FOR ||
		token == LBRACE || token == ID || token == RETURN || token == SCANF || token == PRINTF)
	{
		p->child[0] = statement();
	}
	while (token == IF || token == WHILE || token == FOR ||
		token == LBRACE || token == ID || token == RETURN || token == SCANF || token == PRINTF)
	{
		p->child[0]->sibling = statement();
		p = p->child[0]->sibling;
	}

	return t;
}
TreeNode * statement()
{
	TreeNode *t = NULL;
	switch (token)
	{
	case IF:
		t = if_stmt();
		break;
	case WHILE:
		t = while_stmt();
		break;
	case FOR:
		t = for_stmt();
		break;
	case LBRACE:
		match(LBRACE);
		t = statementlist();
		match(RBRACE);
		break;
	case ID:
		id_reserve = copyString(tokenString);
		t = useFunction();
		if (t == NULL)
		{
			ParseBack = TRUE;
			t = assign_stmt();
		}
		break;
	case RETURN:
		t = return_stmt();
		break;
	case SCANF:
		t = scanf_stmt();
		break;
	case PRINTF:
		t = printf_stmt();
		break;
	default:
		syntaxError("statement error");
		break;

	}
	return t;

}
//＜条件语句＞ :: = if ‘(’＜条件＞‘)’＜语句＞
TreeNode * if_stmt()
{
	TreeNode *t = makeNode(if_stmtK);
	match(IF);
	match(LPAREN);
	t->child[0] = if_exp();
	match(LPAREN);
	t->child[0]->sibling = statement();
	
	return t;
}
TreeNode * if_exp()
{
	TreeNode *t = makeNode(if_expK);
	t->child[0] = exp();
	if (token == LSS || token == LEQ || token == GRE
		|| token == GEQ || token == NOT || token == NEQ)
	{
		TreeNode *p = t;
		p->child[0]->sibling = makeNode(OpK);
		p = p->child[0]->sibling;
		p->sibling = exp();
	}
	return t;
}
TreeNode * while_stmt()
{
	TreeNode *t = makeNode(while_stmtK);
	match(WHILE);
	match(LPAREN);
	t->child[0] = if_exp();
	match(RPAREN);
	t->child[0]->sibling = statement();
	return t;
}
TreeNode * for_stmt()
{
	TreeNode *t = makeNode(for_stmtK);
	match(LPAREN);
	match(ID);
	match(EQ);
	t->child[0] = exp();
	match(SEMI);
	t->child[0]->sibling = if_exp();
	match(SEMI);
	match(ID);
	match(EQ);
	match(ID);
    if(token==PLUS)
	{
		match(PLUS);
		t->attr.val = atoi(tokenString);
		t->child[0]->sibling->sibling = statement();
	}
	else if (token == MINUS)
	{
		match(MINUS);
		t->attr.val = -atoi(tokenString);
		t->child[0]->sibling->sibling = statement();
	}
	else
	{
		syntaxError("for_stmt error!");
	}
	return t;
	
}
TreeNode * useFunction()
{
	TreeNode * t = makeNode(useFunctionK);
	t->attr.name = copyString(tokenString);
	match(ID);
	if (token == LPAREN)
	{
		match(LPAREN);
		t->child[0] = val_parameter();
		match(RPAREN);
	}
	else
	{
		t = NULL;
	}
	return t;
}
TreeNode * val_parameter()
{
	TreeNode *t = NULL;
	if (token == ID)
	{
		TreeNode * t = makeNode(val_parameterK);
		t->child[0] = exp();
		TreeNode *p = t;
		while (token == SEMI)
		{
			match(SEMI);
			p->child[0]->sibling = exp();
			p = p->child[0]->sibling;
		}
	}

	return t;
}
//＜返回语句＞ ::= return[‘(’＜表达式＞‘)’]
TreeNode *return_stmt()
{
	TreeNode *t = makeNode(return_stmtK);
	match(RETURN);
	if (token == LPAREN)
	{
		t->child[0] = exp();
		match(RPAREN);
	}
	else
	{
		t->child[0] = exp();
	}
	return t;
}

//＜读语句＞ :: = scanf ‘(’＜标识符＞{ ,＜标识符＞ }‘)’ 这个语句有点问题
//这样写scanf的功能究竟是什么呢？
TreeNode * scanf_stmt()
{
	TreeNode *t = makeNode(scanf_stmtK);
	match(SCANF);
	match(LPAREN);
	t->attr.name = copyString(tokenString);
	match(ID);
	match(RPAREN);
	return t;

}
TreeNode * printf_stmt()
{
	TreeNode *t = makeNode(printf_stmtK);
	match(PRINTF);
	match(LPAREN);
	t->attr.name = copyString(tokenString);
	match(SSTR);
	match(RPAREN);
	return t;


}
//＜表达式＞ ::= ＜项＞{＜加法运算符＞＜项＞}
TreeNode *exp()
{
	TreeNode *t = makeNode(expK);
	TreeNode *p = t;
	t->child[0] = factor();
	while (token == PLUS || token == MINUS)
	{
		if (token == PLUS)
		{
			match(PLUS);
			p->child[0]->sibling = makeNode(OpK);
			p->child[0]->sibling->attr.op = PLUS;
			p = p->child[0]->sibling;
			p->sibling = factor();


		}
		if (token == MINUS)
		{
			match(MINUS);
			t->child[0]->sibling = makeNode(OpK);
			t->child[0]->sibling->attr.op = MINUS;
			p = p->child[0]->sibling;
			p->sibling = factor();
		}
	}
	return t;
}
/*项*/


/*因子*/
TreeNode *factor()
{
	TreeNode *t = makeNode(factorK);
	if (token == ID)
	{
		t->attr.name = copyString(tokenString);
		match(ID);
		if (token == LBRACKET)
		{
			match(LBRACKET);
			t->nodekind = ArrayK;
			t->child[0] = exp();
			match(RBRACKET);
		}
		else if (token == LPAREN)
		{
			match(LPAREN);
			t->nodekind = useFunctionK;
			t->child[0] = useFunction();

		}
	}
	else if (token == NUM)
	{
		t->attr.val = atoi(tokenString);
		//t->attr.name = NUMK;
		match(NUM);

	}
	else if (token == LPAREN)
	{
		match(LPAREN);
		t->child[0] = exp();
		match(RPAREN);
	}
	return t;
}
//＜赋值语句＞ ::= ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
static TreeNode * assign_stmt()
{
	TreeNode* t = makeNode(OpK);
	t->attr.op = EQ;
	t->child[0] = makeNode(IdK);
	if (ParseBack == TRUE)
	{
		t->child[0]->attr.name = copyString(id_reserve);
		ParseBack = FALSE;
	}
	if (token == LBRACKET)
	{
		match(LBRACKET);
		t->child[0]->nodekind = ArrayK;
		t->child[0]->sibling = exp();
		match(RBRACKET);
	}
	match(EQ);
	t->sibling = exp();
	match(SEMI);
	return t;
}

/*主函数，parse,从开始节点进入递归下降*/
TreeNode * parse()
{
	TreeNode *t;
	token = getToken();//取单词

	t = program();//进入递归下降分析

	if (token != ENDFILE)
		syntaxError("Code ends before file\n");
	if (token == ENDFILE)
	{
		fprintf(listing, "语法分析成功！！！！\n");
	}
	return t;
}