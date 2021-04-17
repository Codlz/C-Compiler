#include <bits/stdc++.h>

#define MAXLEN_NUM 14   //数字最长长度
#define MAXLEN_ID 10    //标识符最长长度

enum state {
	START,NUM,ID,GTR,GEQ,NEQ,LES,LEQ,END,STRING,FLOAT,BECOME,EQUAL
};

char *errorInf[] = {
	"illegal character!", "illegal number!", "identifier is too long!", "string is too long"
};

void outError(int i);	//输出错误信息

void lexer();			//词法分析

bool isSeperator(char c);

bool isOperater(char c);



