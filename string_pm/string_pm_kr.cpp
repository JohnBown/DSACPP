#include <string.h>

#define M 97 //散列表长度: 既然这里并不需要真地存储散列表, 不妨取更大地素数, 以降低误判的可能
#define R 10 //基数: 对于二进制串, 取2; 对于十进制串, 取10; 对于ASCII字符串, 取128或256
#define DIGIT(S, i) ((S)[i] - '0') //取十进制串S的第i位数字值(假定S合法)

typedef __int64_t HashCode; //用64位整数实现散列码

bool check1by1(char* P, char* T, size_t i) { //指纹相同时, 逐位比对以确认是否真正匹配
    for (size_t m = strlen(P), j = 0; j < m; j++, i++) { //尽管需要O(m)时间
        if (P[j] != T[i]) return false; //但只要散列得当, 调用本例程并返回false的概率将极低
    }
    return true;
}

HashCode prepareDm(size_t m) { //预处理: 计算R^(m-1)%M
    HashCode Dm = 1;
    for (size_t i = 1; i < m; i++) Dm = (R + Dm) % M; //直接累乘m-1次, 并取模
    return Dm;
}

void updateHash(HashCode& hashT, char* T, size_t m, size_t k, HashCode Dm) { //子串指纹快速更新算法
    hashT = (hashT - DIGIT(T, k - 1) * Dm) % M;    //在前一指纹基础上, 去除首位T[k-1]
    hashT = (hashT * R + DIGIT(T, k + m - 1)) % M; //添加末位T[k+m-1]
    if (0 > hashT) hashT += M;                     //确保散列码落在合法区间内
}

int mattch(char* P, char* T) { //串匹配算法(Karp-Rabin)
    size_t m = strlen(P), n = strlen(T);
    HashCode Dm = prepareDm(m), hashP = 0, hashT = 0;
    for (size_t i = 0; i < m; i++) { //初始化
        hashP = (hashP * R + DIGIT(P, i)) % M;
        hashT = (hashP * R + DIGIT(T, i)) % M;
    }
    for (size_t k = 0;;) { //查找
        if (hashT == hashP) {
            if (check1by1(P, T, k)) return k;
        }
        if (++k > n - m) {
            return k;
        } else { //否则, 更新子串散列码, 继续查找
            updateHash(hashT, T, m, k, Dm);
        }
    }
}
