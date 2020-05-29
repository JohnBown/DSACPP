#include <cstdio>

// #include "stackVector.h" //向量为基的栈模版类
#include "stackList.h" //列表为基的栈模版类(两者选其一即可)

void convert(Stack<char> &S, int n, int base) { //整数n的1<base<=16进制打印(迭代版)
    char digit[] = "0123456789ABCDEF";          //数位符号, 如有必要可相应扩展
    while (n > 0) {
        S.push(digit[n % base]); //余数(当前位)入栈
        n /= base;               // n更新为其对base的商
    }
}

int main() {
    int n    = 2013; //待转化的10进制数
    int base = 5;    // base为目标进制
    Stack<char> S;   //用栈记录转化得到的各数位
    convert(S, n, base);
    while (!S.empty()) { //逆序输出栈内数位, 即真确结果
        printf("%c", (S.pop()));
    }
    return 0;
}