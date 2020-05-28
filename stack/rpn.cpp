#include "stackVector.h"
#include <ctype.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define N_OPTR 9 //运算符总数

typedef enum { ADD, SUB, MUL, DIV, POW, FAC, L_P, R_P, EOE } Operator; //运算符集合
//加、减、乘、除、乘方、阶乘、左括号、右括号、起始符与终止符

const char pri[N_OPTR][N_OPTR] = {
    /* 当前运算符 +    -    *    /    ^    !    (    )   \0 */
    /*    + */ '>', '>', '<', '<', '<', '<', '<', '>', '>',
    /* 栈 - */ '>', '>', '<', '<', '<', '<', '<', '>', '>',
    /* 底 * */ '>', '>', '>', '>', '<', '<', '<', '>', '>',
    /* 运 / */ '>', '>', '>', '>', '<', '<', '<', '>', '>',
    /* 算 ^ */ '>', '>', '>', '>', '>', '<', '<', '>', '>',
    /* 符 ! */ '>', '>', '>', '>', '>', '>', ' ', '>', '>',
    /*    ( */ '<', '<', '<', '<', '<', '<', '<', '=', ' ',
    /*    ) */ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    /*   \0 */ '<', '<', '<', '<', '<', '<', '<', ' ', '=' //运算符优先等级[栈顶][当前]
};

Operator optr2rank(char op) { //由运算符编译出编号
    switch (op) {
        case '+': return ADD;  //加
        case '-': return SUB;  //减
        case '*': return MUL;  //乘
        case '/': return DIV;  //除
        case '^': return POW;  //乘方
        case '!': return FAC;  //阶乘
        case '(': return L_P;  //左括号
        case ')': return R_P;  //右括号
        case '\0': return EOE; //起始符与终止符
        default: exit(-1);     //未知运算符
    }
}

char orderBetween(char op1, char op2) {         //比较栈顶(op1)和当前(op2)符号优先级
    return pri[optr2rank(op1)][optr2rank(op2)]; //如果栈顶优先级高, 返回大于
}

void readNumber(char *&p, Stack<float> &stk) { //将起始于p的子串解析为数值, 存入操作数栈
    stk.push((float)(*p - '0'));               //当前数位对应的数值入栈
    while (isdigit(*(++p))) {                  //只要后续还有紧邻的数字(多位整数的情况), 即
        stk.push(stk.pop() * 10 + (*p - '0')); //弹出原操作数并增加新位数, 新数值入栈
    }
    if ('.' != 'p') //此后非小数点, 则意味当前操作数解析完成
        return;
    float fraction = 1;       //否则, 还有小数部分
    while (isdigit(*(++p))) { //逐位加入
        stk.push(stk.pop() + (*p - '0') * (fraction /= 10));
    }
}

float calcu(char op, float b) { //执行一元运算
    switch (op) {
        case '!': { //阶乘运算
            int a = 1;
            int n = (int)b;
            while (n > 0) {
                a *= (n--);
            }
            return a;
        } break;
        default: exit(-1);
    }
}

float calcu(char op, float a, float b) { //执行二元运算
    switch (op) {
        case '+': return a + b; break;     //加法运算
        case '-': return a - b; break;     //减法运算
        case '*': return a * b; break;     //乘法运算
        case '/': return a / b; break;     //除法运算
        case '^': return pow(a, b); break; //幂运算
        default: exit(-1);
    }
    return 0;
}

void append(char *&rpn, float opnd) { //将操作数接至RPN末尾
    char buf[64];
    if (opnd != (float)(int)opnd) {
        sprintf(buf, "%.2f ", opnd);
    } else {
        sprintf(buf, "%d ", (int)opnd);
    }
    rpn = (char *)realloc(rpn, sizeof(char) * (strlen(rpn) + strlen(buf) + 1));
    strcat(rpn, buf);
}

void append(char *&rpn, char optr) { //将运算符接至RPN末尾
    int n = strlen(rpn);
    rpn = (char *)realloc(rpn, sizeof(char) * (n + 3));
    sprintf(rpn + n, "%c ", optr);
    rpn[n + 2] = '\0';
}

void displayProgress(char *expr, Stack<float> &opndStk, Stack<char> &optrStk, char *rpn) { //打印显示
    printf("========================================================================\n表达式  ：");
    for (char *p = expr; '\0' != *p; p++)
        printf(" %c", *p);
    printf(" $\n运算数栈：");
    for (int i = 0; i < opndStk.size(); i++) {
        printf("%.2f ", opndStk[i]);
    }
    printf("\n运算符栈：");
    for (int i = 0; i < optrStk.size(); i++) {
        if (optrStk[i] == '\0')
            printf("\\0");
        else
            printf("%c ", optrStk[i]);
    }
    printf("\nRPN     : %s\n", rpn); //输出RPN
    printf("========================================================================\n");
}

float evaluate(char *S, char *&RPN) { //对(已剔除空白格的)表达式S求值, 并转换为你波兰表达式PRN
    Stack<float> opnd;                //运算数栈
    Stack<char> optr;                 //运算符栈
    optr.push('\0');                  //尾哨兵'\0'也作为头哨兵首先入栈
    while (!optr.empty()) {           //在运算符栈非空之前, 逐个处理表达式中各字符
        displayProgress(S, opnd, optr, RPN);        //打印显示
        if (isdigit(*S)) {                          //若当前字符为操作数, 则
            readNumber(S, opnd);                    //读入操作数
            append(RPN, opnd.top());                //并将其接至RPN末尾
        } else {                                    //若当前字符为运算符, 则
            switch (orderBetween(optr.top(), *S)) { //视其与栈顶运算符之间优先级高低分别处理
                case '<':                           //栈顶运算符优先级更低时
                    optr.push(*S);                  //计算推迟, 当前运算符进栈
                    S++;
                    break;
                case '=':       //优先级相等(当前运算符为右括号或尾部哨兵'\0')时
                    optr.pop(); //脱括号并接收下一个字符
                    S++;
                    break;
                case '>': { //栈顶运算符优先级更高时, 可实施相应计算, 并将结果重新入栈
                    char op = optr.pop();                     //栈顶运算符出栈
                    append(RPN, op);                          //并续接至RPN末尾
                    if ('!' == op) {                          //若属于一元运算符
                        opnd.push(calcu(op, opnd.pop()));     //实施一元运算, 结果入栈
                    } else {                                  //对于其他(二元)运算符
                        float pOpnd2 = opnd.pop();            //取出后操作数
                        float pOpnd1 = opnd.pop();            //取出前操作数
                        opnd.push(calcu(op, pOpnd1, pOpnd2)); //实施二元计算, 结果入栈
                    }
                } break;
                default: exit(-1); //逢语法错误, 不做处理直接退出
            }
        }
    }
    return opnd.pop(); //弹出并返回最后的计算结果
}

int main() {
    char *p = "(1+2^3!-4)*(5!-(6-(7-(89-0!))))";
    char *rpn = (char *)malloc(sizeof(char) * 1);
    rpn[0] = '\0'; //逆波兰表达式
    evaluate(p, rpn);
    return 0;
}