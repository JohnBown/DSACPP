#include <cstdio>
#include <math.h>

#include "../stack/stackVector.h"

int counts[101];

void solve(int n) {
    int N = (int)sqrt(n); //剪枝
    int q = 0, sum = 0;   //从第一个自然数开始
    Stack<int> solu;      //存放解的栈

    do {
        if (4 <= solu.size() || N < q) { //栈溢出, 或者数值溢出
            q = solu.pop();              //回溯
            sum -= q * q;
            q++;
        } else {
            if (q <= N && sum + q * q <= n) { //否则
                solu.push(q);                 //向栈中推入数据
                sum += q * q;
                if ((solu.size() == 4)) { //每达到四分解
                    counts[sum]++;        //统计数字加一
                    if (sum == n) {       //对n的情况, 输出分解式子
                        printf("%d = %d^2 + %d^2 + %d^2 + %d^2\n", n, solu[0], solu[1], solu[2], solu[3]);
                    }
                }
            } else { // q <= N && sum + q*q >n
                q = N + 1;
            }
        }
    } while (0 < solu.size() || q <= N);
}

int main() {
    solve(4);
    for (int i = 0; i < 101; i++) {
        printf("n: %d, 有%d组\n", i, counts[i]);
    }
}