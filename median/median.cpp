#include "../vector/vector.h"

template <typename T>
T trivialMedian(Vector<T>& S1, int lo1, int n1, Vector<T>& S2, int lo2,
                int n2) { //有序向量归并后中位数(蛮力版)
    int hi1 = lo1 + n1;
    int hi2 = lo2 + n2;
    Vector<T> S; //将两个有序子向量归并为一个有序向量
    while ((lo1 < hi1) && (lo2 < hi2)) {
        while ((lo1 < hi1) && S1[lo1] <= S2[lo2]) S.insert(S1[lo1++]);
        while ((lo2 < hi2) && S2[lo2] <= S1[lo1]) S.insert(S2[lo2++]);
    }
    while (lo1 < hi1) S.insert(S1[lo1++]);
    while (lo2 < hi2) S.insert(S2[lo2++]);
    return S[(n1 + n2) / 2]; //直接返回归并向量中位数
}

template <typename T>
T median(Vector<T>& S1, int lo1, Vector<T>& S2, int lo2, int n) { //等长有序向量归并后中位数算法
    if (n < 3) return trivialMedian(S1, lo1, n, S2, lo2, n);      //递归基
    int mi1 = lo1 + n / 2;                                        //长度(接近)减半
    int mi2 = lo2 + (n - 1) / 2;
    if (S1[mi1] < S2[mi2]) {
        return median(S1, mi1, S2, lo2, n + lo1 - mi1); //取S1右半, S2左半
    } else if (S1[mi1] > S2[mi2]) {
        return median(S1, lo1, S2, mi2, n + lo2 - mi2); //取S1左半, S2右半
    } else {
        return S1[mi1];
    }
}

template <typename T>
T median(Vector<T>& S1, int lo1, int n1, Vector<T>& S2, int lo2, int n2) { //不等长有序向量归并后中位数算法
    if (n1 > n2) return median(S1, lo2, n2, S1, lo1, n1);                  //确保n1<=n2
    if (n2 < 6) return trivialMedian(S1, lo1, n1, S2, lo2, n2); //递归基
    if (2 * n1 < n2) { //若两个向量的长度相差悬殊, 则长者(S2)的两翼可直接截除
        return median(S1, lo1, n1, S2, lo2 + (n2 - n1 - 1) / 2, n1 + 2 - (n2 - n1) % 2);
    }
    int mi1  = lo1 + n1 / 2;
    int mi2a = lo2 + (n1 - 1) / 2;
    int mi2b = lo2 + n2 - 1 - n1 / 2;
    if (S1[mi1] > S2[mi2b]) { //取S1左半, S2右半
        return median(S1, lo1, n1 / 2 + 1, S2, mi2a, n2 - (n1 - 1) / 2);
    } else if (S1[mi1] < S2[mi2a]) { //取S1右半, S2左半
        return median(S1, mi1, (n1 + 1) / 2, S2, lo2, n2 - n1 / 2);
    } else { // S1保留, S2左右同时缩短
        return median(S1, lo1, n1, S2, mi2a, n2 - (n1 - 1) / 2 * 2);
    }
}