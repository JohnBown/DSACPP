#include <string.h>

int match(char *P, char *T) {             //串匹配算法(Brute-force-2)
    size_t n = strlen(T), i = 0;          //文本串长度, 当前接受比对字符的位置
    size_t m = strlen(P), j;              //模式串长度, 当前接受比对字符的位置
    for (int i = 0; i < n - m + 1; i++) { //文本串从第i个字符起, 与
        for (j = 0; j < m; j++) {         //模式串中对应的字符逐个比对
            if (T[i + j] != P[j]) break;  //若失配, 模式串整体右移一个字符, 再做一轮比对
        }
        if (j >= m) break; //找到匹配子串
    }
    return i; //成功)返回匹配字符串首位; 失败)返回一个异常位置
}