#include "dfa.h"

using namespace std;

char ch;        //上次读到的字符
int currentState = START;
FILE *src;      //源文件
FILE *output;   //标准输出
FILE *idout;   //变量和函数
FILE *unum;     //无符号数

char aops[] = {'+','-','*','/','%'};				//算术运算符
char sep[] = {';','{','}','(',')','[',']','.',',','#'};	//分隔符
unordered_set<string> resWord({
    "include","define","auto","bool","break","case","catch","char","class",

    "const","const_cast","continue","default","delete","do","double",

    "dynamic_cast","else","enum","explicit","extern","false","float","for",

    "friend","goto","if","inline","int","long","mutable","namespace","new",

    "operator","private","protected","public","register","reinterpret_cast",

    "return","short","signed","sizeof","static","static_cast","struct",

    "switch","template","this","throw","true","try","typedef","typeid",

    "typename","union","unsigned","using","virtual","void","volatile","while"
});
unordered_set<string> idset;

void outError(int i)
{
    fprintf(output, "%s\n", errorInf[i - 1]);
    exit(0);
}

bool isOperator(char c)
{
    for (int i = 0; i < sizeof(aops); i++) {
        if (c == aops[i])
            return true;
    }
    return false;
}

bool isSeperator(char c)
{
    for (int i = 0; i < sizeof(sep); i++) {
        if (c == sep[i])
            return true;
    }
    return false;
}


void lexer()
{
    float num = 0;
    int digitCount = 0; //有多少位数字
    int div = 10;   //用于计算小数
    string id;  //标识符
    string str; //字符串

    ch = fgetc(src); //获取文件第一个字符

    while (ch != EOF)
    {
        switch (currentState)
        {
        case START:
            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
            { 
                
            }
            else if (ch == '"')
            { 
                //字符串
                currentState = STRING;
            }
            else if (isdigit(ch))
            {
                currentState = NUM;
                num = ch - '0';
                digitCount++;
            }
            else if (isalpha(ch) or ch == '_')
            {
                currentState = ID;
                id.push_back(ch);
            }
            else if (ch == '=') {
                currentState = BECOME;
            }
            else if (ch == '>')
                currentState = GTR;
            else if (ch == '<')
                currentState = LES;
            else
            { 
                //单独字符直接识别
                if (isOperator(ch)) {
                    fprintf(output, "<Operator, %c>\n", ch);
                }
                else if (isSeperator(ch)) {
                    fprintf(output, "<Seperator, %c>\n", ch);
                }
                else {
                    outError(1);
                }
            }
            break;

        case NUM:
            if (isdigit(ch))
            {   
                if (digitCount >= MAXLEN_NUM || (digitCount == 1 && num == 0)) {
                    outError(2);    //非法数字
                }
                num = num * 10 + ch - '0';
                digitCount++;
            }
            else if (ch == '.') {
                currentState = FLOAT;
            }
            else
            { //token识别完毕
                ch = ungetc(ch, src); // 回退该字符，重新识别
                //输出
                fprintf(output, "<Number, %d>\n", (int)num);
                fprintf(unum, "<Number, %f>\n", num);
                currentState = START;
                num = 0;
                digitCount = 0;
            }
            break;
        
        case BECOME:
            if (ch == '=') {
                fprintf(output, "<Roperator, ==>\n", (int)num);
                currentState = START;
            }
            else {
                ungetc(ch, output);
                fprintf(output, "<Operator, =>\n", (int)num);
                currentState = START;
            }
            break;

        case FLOAT:
            if (isdigit(ch)) {
                num += (ch - '0') / (float)div;
                div *= 10;
            }
            else {
                ungetc(ch, src);
                fprintf(output, "<Number, %f>\n", num);
                fprintf(unum, "<Number, %f>\n", num);
                currentState = START;
                num = 0;
                div = 10;
                digitCount = 0;
            }
            break;

        case ID:
            if (isalpha(ch) || isdigit(ch) || ch == '_')
            {
                if (id.length() >= MAXLEN_ID)
                {
                    outError(3);
                }
                id.push_back(ch);
            }
            else
            { //token识别完毕
                currentState = START;
                ch = ungetc(ch, src); // 回退该字符，重新识别

                // 检查是否为保留字
                if (resWord.count(id) != 0) {
                    //输出
                    fprintf(output, "<Reserve, %s>\n", id.c_str());
                }
                else {
                    //输出
                    if (idset.find(id) == idset.end()) {
                        idset.insert(id);
                    }
                    fprintf(output, "<Id, %s>\n", id.c_str());
                }
                id.clear();
            }
            break;

        case GTR:
            if (ch == '=')
            {
                currentState = GEQ; //遇到大于等于号
            }
            else
            { //token识别完毕
                currentState = START;
                ch = ungetc(ch, src); // 回退该字符，重新识别
                //输出
                fprintf(output, "<Roperator, >>\n", ch);
            }
            break;

        case LES:
            if (ch == '=')
            {
                currentState = LEQ;
            }
            else
            { //token识别完毕
                currentState = START;
                ch = ungetc(ch, src); // 回退该字符，重新识别
                //输出
                fprintf(output, "<Roperator, <>\n", ch);
            }
            break;

        case GEQ: //token识别完毕
            currentState = START;
            ch = ungetc(ch, src); // 回退该字符，重新识别
            //输出
            fprintf(output, "<Roperator, >=>\n");
            break;

        case LEQ: //token识别完毕
            currentState = START;
            ch = ungetc(ch, src); // 回退该字符，重新识别
            //输出
            fprintf(output, "<Roperator, <=>\n");
            break;
        case STRING:
            if (ch == '"') {
                fprintf(output, "<String, %s>\n", str.c_str());
                currentState = START;
                str.clear();
            }
            else {
                str.push_back(ch);
                if (str.length() > 400) {
                    outError(4);
                }
            }
            break;
        }
            
        //在最后获取下一个字符
        ch = fgetc(src);
    }
}
 
int main() 
{   
    src = fopen("./C++/C PROGRAM/cAnalysis/src.cpp", "r");
    output = fopen("./C++/C PROGRAM/cAnalysis/output.text", "w");
    idout = fopen("./C++/C PROGRAM/cAnalysis/idlist.txt", "w");
    unum = fopen("./C++/C PROGRAM/cAnalysis/unilist.txt", "w");

    if (!src) {
        printf("打开文件失败");
    }
    else {
        lexer();
    }

    return 0;
}
