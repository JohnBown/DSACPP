#include <string.h>

int match(char* P, char* T) {    //串匹配算法(Brute-force-1)
    size_t n = strlen(T), i = 0; //文本串长度, 当前接受比对字符的位置
    size_t m = strlen(P), j = 0; //模式串长度, 当前接受比对字符的位置
    while (j < m && i < n) {     //自左向右逐个比对字符
        if (T[i] == P[j]) {      //若匹配
            i++;                 //转到下一对字符
            j++;
        } else {        //否则
            i -= j - 1; //文本串回退
            j = 0;      //模式串复位
        }
    }
    return i - j; //成功)返回匹配字符串首位; 失败)返回一个异常位置
}