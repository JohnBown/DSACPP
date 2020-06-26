#include <string.h>

int *buildNext_old(char *P) {    //构造模式串的next表(待改进版本)
    size_t m = strlen(P), j = 0; //"主"串指针
    int *N = new int[m];         // next表
    int t = N[0] = -1;           //模式串指针
    while (j < m - 1) {
        if (0 > t || P[j] == P[t]) { //匹配
            j++;
            t++;
            N[j] = t; //**此句可改进
        } else {      //失配
            t = N[t];
        }
    }
    return N;
}

int *buildNext(char *P) {        //构造模式串的next表(改进版本)
    size_t m = strlen(P), j = 0; //"主"串指针
    int *N = new int[m];         // next表
    int t = N[0] = -1;           //串模式指针
    while (j < m - 1) {
        if (0 > t || P[j] == P[t]) { //匹配
            j++;
            t++;
            N[j] = (P[j] != P[t] ? t : N[t]); //**注意此句与未改进之前的区别
        } else {                              //失配
            t = N[t];
        }
    }
    return N;
}

int match(char *P, char *T) {        //串匹配算法(KMP)
    int *next = buildNext(P);        //构造next表
    int n = (int)strlen(T), i = 0;   //文本串指针
    int m = (int)strlen(P), j = 0;   //模式串指针
    while (j < m && i < n) {         //自左向右逐个比对字符
        if (0 > j || T[i] == P[j]) { //若匹配, 或p已移出最左侧(两个判断次序不可交换)
            i++;                     //则转移到下一字符
            j++;
        } else {         //否则
            j = next[j]; //模式串右移(注意: 文本串不用回退)
        }
    }
    delete[] next; //释放next表
    return i - j;
}
