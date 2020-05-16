#include <iostream>

#include "stackVector.h"

using namespace std;

bool stack_paren(const char exp[], int lo, int hi) { //表达式括号匹配检查，兼顾三种括号
    Stack<char> S;                                   //使用栈记录已发现但尚未匹配的左括号
    for (int i = lo; i < hi; i++) {                  //逐一检查当前字符
        switch (exp[i]) {
        case '(':
        case '[':
        case '{':
            S.push(exp[i]); //左括号入栈
            break;
        case ')': //右括号若与栈顶失配，则表达式必不匹配
            if ((S.empty()) || ('(' != S.pop()))
                return false;
            break;
        case ']':
            if ((S.empty()) || ('[' != S.pop()))
                return false;
            break;
        case '}':
            if ((S.empty()) || ('{' != S.pop()))
                return false;
            break;
        default:
            break; //非括号字符一律忽略
        }
    }
    return S.empty(); //整个表达式扫描后，整个栈还残留（左）括号，则不匹配，否则（栈空）匹配
}

bool recursive_paren(const char exp[], int lo, int hi) {
    // TODO
}

int main() {
    char exp0[] = "(a[i - 1][j + 1]) + a[i + 1][j - 1]) * 2";
    char exp1[] = "(a[i - 1][j + 1] + a[i + 1][j - 1]) * 2";

    cout << stack_paren(exp0, 0, 40) << endl;
    cout << stack_paren(exp1, 0, 39) << endl;

    return 0;
}