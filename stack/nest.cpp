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

void trim(const char exp[], int &lo, int &hi) { //删除exp[lo, hi]不含括号的最长前缀、后缀
    while ((lo <= hi) && (exp[lo] != '(') && (exp[lo] != ')'))
        lo++;
    while ((lo <= hi) && (exp[hi] != '(') && (exp[hi] != ')'))
        hi--;
}

int divide(const char exp[], int lo, int hi) { //切分exp[lo, hi]，使exp匹配当且仅当子表达式匹配
    int mi = lo;
    int crc = 1;
    while ((0 < crc) && (++mi < hi)) {
        if (exp[mi] == ')')
            crc--;
        if (exp[mi] == '(')
            crc++;
    }
    return mi;
}

bool recursive_paren(const char exp[], int lo, int hi) { //括号匹配，递归版本
    trim(exp, lo, hi);
    if (lo > hi)
        return true;
    if (exp[lo] != '(')
        return false;
    if (exp[hi] != ')')
        return false;
    int mi = divide(exp, lo, hi);
    if (mi > hi)
        return false;
    return recursive_paren(exp, lo + 1, mi - 1) && recursive_paren(exp, mi + 1, hi);
}

int main() {
    char exp0[] = "(a[i - 1][j + 1]) + a[i + 1][j - 1]) * 2";
    char exp1[] = "(a[i - 1][j + 1] + a[i + 1][j - 1]) * 2";

    cout << stack_paren(exp0, 0, 40) << endl;
    cout << stack_paren(exp1, 0, 39) << endl;

    cout << recursive_paren(exp0, 0, 40) << endl;
    cout << recursive_paren(exp1, 0, 39) << endl;

    return 0;
}