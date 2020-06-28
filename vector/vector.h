#ifndef VECTOR_H
#define VECTOR_H

typedef int Rank;
#define DEFAULT_CAPACITY 3 //默认初试容量

template <typename T> class Vector { //向量模版类
  private:
    Rank partition_A(Rank lo, Rank hi); //轴点构造算法(版本A)
    Rank partition_B(Rank lo, Rank hi); //轴点构造算法(版本B)
    Rank partition_C(Rank lo, Rank hi); //轴点构造算法(版本C)

  protected:
    Rank _size;                                  //规模
    int _capacity;                               //容量
    T *_elem;                                    //数据区
    void copyFrom(T const *A, Rank lo, Rank hi); //复制数组区间[lo, hi)
    void expand();                               //空间不足时扩容
    void shrink();                         //(装填因子: _size/_capacity)过小时压缩向量所占空间
    Rank bubble(Rank lo, Rank hi);         //扫描交换
    void bubbleSort(Rank lo, Rank hi);     //起泡排序算法
    Rank max(Rank lo, Rank hi);            //选取最大元素
    void selectionSort(Rank lo, Rank hi);  //选择排序算法
    void insertionSort(Rank lo, Rank hi);  //插入排序算法
    void merge(Rank lo, Rank mi, Rank hi); //归并算法
    void mergeSort(Rank lo, Rank hi);      //归并排序算法
    void heapSort(Rank lo, Rank hi);       //堆排序算法
    Rank partition(Rank lo, Rank hi);      //轴点构造算法
    void quickSort(Rank lo, Rank hi);      //快速排序算法
    void shellSort(Rank lo, Rank hi);      //希尔排序算法

  public:
    //构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) { //容量为c, 规模为s, 所有元素初始为v
        _elem = new T[_capacity = c];
        for (_size = 0; _size < s; _size++) { _elem[_size] = v; }
    }
    Vector(T const *A, Rank n) { copyFrom(A, 0, n); }                           // A[0, n)
    Vector(T const *A, Rank lo, Rank hi) { copyFrom(A, lo, hi); }               // A[lo, hi)
    Vector(Vector<T> const &V) { copyFrom(V._elem, 0, V._size); }               // V[0, V._size)
    Vector(Vector<T> const &V, Rank lo, Rank hi) { copyFrom(V._elem, lo, hi); } // V[lo, hi)
    //析构函数
    ~Vector() { delete[] _elem; } //释放内部空间
    //只读访问接口
    Rank size() const { return _size; }                       //规模
    bool empty() const { return !_size; }                     //判空
    Rank find(T const &e) const { return find(e, 0, _size); } //无序向量整体查找
    Rank find(T const &e, Rank lo, Rank hi) const;            //无序向量区间查找
    Rank search(T const &e) const { return (0 >= _size) ? -1 : search(e, 0, _size); } //有序向量整体查找
    Rank search(T const &e, Rank lo, Rank hi) const; //有序向量区间查找
    //可写访问接口
    T &operator[](Rank r);                               //重载下标运算符
    const T &operator[](Rank r) const;                   //限于做右值运算的重载版本
    Vector<T> &operator=(Vector<T> const &V);            //重载赋值运算符
    T remove(Rank r);                                    //删除秩为r的元素
    int remove(Rank lo, Rank hi);                        //删除秩在区间[lo, hi)之内的元素
    Rank insert(Rank r, T const &e);                     //插入元素
    Rank insert(T const &e) { return insert(_size, e); } //默认作为末元素插入
    void sort(Rank lo, Rank hi);                         //对[lo, hi)排序
    void sort() { sort(0, _size); }                      //整体排序
    void unsort(Rank lo, Rank hi);                       //对[lo, hi)置乱
    void unsort() { unsort(0, _size); }                  //整体置乱
    int deduplicate();                                   //无序向量去重
    int uniquify();                                      //有序向量去重
    //遍历
    void traverse(void (*)(T &));                 //遍历(使用函数指针, 只读或局部性修改)
    template <typename VST> void traverse(VST &); //遍历(使用函数对象, 可全局性修改)
};

template <typename T> void Vector<T>::copyFrom(T const *A, Rank lo, Rank hi) { //复制数组区间[lo, hi)
    _size = 0;
    _elem = new T[_capacity = 2 * (hi - lo)]; //初试双倍规模的容量
    while (lo < hi) {                         // A[lo, hi)内的元素逐一
        _elem[_size++] = A[lo++];             //复制至_elem[0, hi - lo)
    }
}

template <typename T> T &Vector<T>::operator[](Rank r) { return _elem[r]; } //重载下标操作符

template <typename T> const T &Vector<T>::operator[](Rank r) const { return _elem[r]; } //限右值

template <typename T> Vector<T> &Vector<T>::operator=(Vector<T> const &V) { //重载赋值运算符
    if (_elem) delete[] _elem;                                              //释放原有内容
    copyFrom(V._elem, 0, V.size());
    return *this; //返回当前对象的引用, 以便链式赋值
}

template <typename T> void Vector<T>::expand() {                    //向量空间不足时, 扩容
    if (_size < _capacity) return;                                  //尚未满员, 不必扩容
    if (_capacity < DEFAULT_CAPACITY) _capacity = DEFAULT_CAPACITY; //不低于最小容量

    T *oldElem = _elem;
    _elem      = new T[_capacity << 1];                    //倍增
    for (int i = 0; i < _size; i++) _elem[i] = oldElem[i]; //范型T已重载赋值运算符

    delete[] oldElem; //释放原空间
}

template <typename T> void Vector<T>::shrink() {   //装填因子过小时压缩空间
    if (_capacity < DEFAULT_CAPACITY << 1) return; //不缩至默认容量以下
    if (_size << 2 > _capacity) return;            //以25%为界

    T *oldElem = _elem;
    _elem      = new T[_capacity >>= 1];                   //减半
    for (int i = 1; i < _size; i++) _elem[i] = oldElem[i]; // T为基本类型或已重载赋值运算符

    delete[] oldElem; //释放原空间
}

template <typename T> Rank Vector<T>::find(T const &e, Rank lo, Rank hi) const {
    while ((lo < hi--) && (e != _elem[hi])) {}; //从后向前查找, 当同时多个命中时, 自动返回秩最大者
    return hi;                                  //若hi < lo, 则意味着失败
}

template <typename T> Rank Vector<T>::insert(Rank r, T const &e) { //插入e为秩r
    expand();                                                      //若有必要, 扩容
    for (int i = _size; r < i; i--) _elem[i] = _elem[i - 1]; //自后向前, 后继元素顺次后移一个单元

    _elem[r] = e; //插入新元素e
    _size++;      //更新容量
    return r;     //返回秩
}

template <typename T> T Vector<T>::remove(Rank r) { //删除秩为r的元素
    T e = _elem[r];                                 //备份被删除的元素
    remove(r, r + 1);                               //对区间[r, r+1)的元素删除
    return e;                                       //返回被删除元素
}

template <typename T> int Vector<T>::remove(Rank lo, Rank hi) { //区间删除
    if (lo == hi) return 0;                                     //单独处理退化情况
    while (hi < _size) _elem[lo++] = _elem[hi++];               //[hi, _size)顺次前移hi - lo个单元
    _size = lo;     //更新规模, 直接丢弃尾部[lo, _size = hi)区间
    shrink();       //若有必要, 则缩容
    return hi - lo; //返回被删除元素的数目
}

template <typename T> int Vector<T>::deduplicate() { //删除无序向量中重复元素
    int oldSize = _size;                             //记录原规模
    Rank i      = 1;
    while (i < _size) (find(_elem[i], 0, i) < 0) ? i++ : remove(i); //自前向后逐一考察, 删除_elem[i]；否则i++
    return oldSize - _size;                                         //返回被删除的元素总数
}

template <typename T> int Vector<T>::uniquify() { //有序向量重复元素剔除算法
    int i = 0, j = 0;               //不变性：[0, i]无重复, [i+1, j]必然与前区间重复
    while (++j < _size) {           //逐一扫面, 直至末元素
        if (_elem[i] != _elem[j]) { //跳过雷同者
            _elem[++i] = _elem[j];  //发现不同, 更新i的位置
        }
    }
    _size = ++i;  //区间为左闭右开, 所以你为++i
    shrink();     //直接剔除尾部多余元素
    return j - i; //返回被删除原属
}

template <typename T> void Vector<T>::traverse(void (*visit)(T &)) { //借助函数指针机制
    for (int i = 0; i < _size; i++) visit(_elem[i]);                 //遍历向量
}

template <typename T> template <typename VST> void Vector<T>::traverse(VST &visit) { //借助函数对象机制
    for (int i = 0; i < _size; i++) visit(_elem[i]);                                 //遍历向量
}

template <typename T> static Rank binSearch(T *A, T const &e, Rank lo, Rank hi) {
    while (lo < hi) {                        //一次比较, 两个分支
        Rank mi          = (lo + hi) >> 1;   //以中点为轴点
        (e < A[mi]) ? hi = mi : lo = mi + 1; //[lo, mi)或(mi, hi)
    }
    return --lo; //循环结束时, lo为大于e的元素的最小秩, 故lo -
                 // 1即不大于e的元素的最大秩
} //有多元素命中时, 总能保证返回秩最大者；查找失败时, 能够返回失败的位置

#include "../fibonacci/Fib.h"
template <typename T> static Rank fibSearch(T *A, T const &e, Rank lo, Rank hi) {
    Fib fib(hi - lo);
    while (lo < hi) {
        while (hi - lo < fib.get()) fib.prev();
        Rank mi = fib.get() - 1;
        if (e < A[mi]) {
            hi = mi;
        } else if (A[mi] < e) {
            lo = mi + 1;
        } else {
            return mi;
        }
    }
    return -1;
}

template <typename T>
Rank Vector<T>::search(T const &e, Rank lo, Rank hi) const { //搜索确定不大于e的最后一个节点的秩
    return binSearch(_elem, e, lo, hi);                      //采用二分查找
    // return fibSearch(_elem, e, lo, hi); //采用Fibonaci查找
}

template <typename T> static void swap(T &a, T &b) { //交换
    T temp = a;
    a      = b;
    b      = temp;
}

template <typename T> Rank Vector<T>::bubble(Rank lo, Rank hi) {
    Rank last = lo;                         //最右侧的相邻逆序对的秩, 初始为[lo-1, lo]
    while (++lo < hi) {                     //区间[1, _size), 逐一检查
        if (_elem[lo - 1] > _elem[lo]) {    //发现相邻逆序对
            last = lo;                      //更新last
            swap(_elem[lo - 1], _elem[lo]); //交换
        }
    }
    return last; //返回最右侧逆序对的秩
}

template <typename T> void Vector<T>::bubbleSort(Rank lo, Rank hi) { //向量冒泡排序算法
    while (lo < (hi = bubble(lo, hi))) {} //如果数组后缀部分有序, 那么只迭代无序部分, 即[lo, last]
}

template <typename T> Rank Vector<T>::max(Rank lo, Rank hi) { //在[lo, hi]找出最大值
    Rank mx = hi;
    while (lo < hi--) {                     //逆向扫描
        if (_elem[mx] < _elem[hi]) mx = hi; //严格比较, 故在max有多个时保证后者优先
    }                                       //进而保证selectoniSort稳定
    return mx;
}

template <typename T> void Vector<T>::selectionSort(Rank lo, Rank hi) { //向量选择排序算法
    while (lo < --hi) { swap(_elem[max(lo, hi)], _elem[hi]); }
}

template <typename T> void Vector<T>::insertionSort(Rank lo, Rank hi) { //向量插入排序算法
    for (int i = lo; i < hi; i++) {             //在区间[lo, hi)上, 正向进行迭代
        T key = _elem[i];                       //取出此轮插入的元素, 防止覆盖
        int j = i - 1;                          //索引j从右向左依次进行比较
        while ((j >= lo) && (_elem[j] > key)) { //保证稳定性, 查找出不大于key的最大位置
            _elem[j + 1] = _elem[j];            //还未查找成功
            j--;                                //向右平移你覆盖
        }
        _elem[j + 1] = key; //找到时插入key, key为最小值, 插入lo的位置
    }
}

template <typename T> void Vector<T>::merge(Rank lo, Rank mi, Rank hi) {
    T *A   = _elem + lo; // A[0, hi - lo) = _elem[lo, hi)
    int lb = mi - lo;
    T *B   = new T[lb];            // B[0, lb) = _elem[lo, mi)
    for (int i = 0; i < lb; i++) { //拷贝B的副本, C不需要拷贝
        B[i] = A[i];
    }
    int lc = hi - mi;
    T *C   = _elem + mi; // C[0, lc) = _elem[mi, hi)

    for (Rank i = 0, j = 0, k = 0; (j < lb) || (k < lc);) { //初始化, A[i] = min(B[j], C[k])
        if ((j < lb) && ((lc <= k) || (B[j] <= C[k]))) A[i++] = B[j++];
        if ((k < lc) && ((lb <= j) || (C[k] < B[j]))) A[i++] = C[k++];
    }

    delete[] B; //释放临时空间B
}

template <typename T> void Vector<T>::mergeSort(Rank lo, Rank hi) { //向量归并排序算法
    if (hi - lo < 2) return;                                        //单元素区间自然有序
    Rank mi = (lo + hi) / 2;                                        //以中点为界限
    mergeSort(lo, mi);                                              //递归
    mergeSort(mi, hi);                                              //递归
    merge(lo, mi, hi);                                              //归并
}

#include "../pq_complheap/pq_complHeap.h"
template <typename T> void Vector<T>::heapSort(Rank lo, Rank hi) { //向量堆排序算法
    PQ_ComplHeap<T> H(_elem + lo, hi - lo); //将待排序区间建成一个完全二叉堆, O(n)
    while (!H.empty()) {          //反复地摘除最大元素并归入已排序地后缀, 直至堆空
        _elem[--hi] = H.delMax(); //等效于堆顶于末元素对换后下滤
    }
}

#include <stdlib.h>
template <typename T> Rank Vector<T>::partition_A(Rank lo, Rank hi) { //轴点构造算法(版本A)
    swap(_elem[lo], _elem[lo + rand() % (hi - lo + 1)]);              //任选一个元素与首元素交换
    T pivot = _elem[lo]; //以首元素为候选轴点: 经以上交换, 等效于随机选取
    while (lo < hi) {    //从向量对两端交替地向中间扫描
        while ((lo < hi) && (pivot <= _elem[hi])) hi--; //在不小于pivot的前提下, 向左拓展右端子向量
        _elem[lo] = _elem[hi];                          //小于pivot者归入左侧子序列
        while ((lo < hi) && (_elem[lo] <= pivot)) lo++; //在不大于pivot的前提下, 向右拓展左端子向量
        _elem[hi] = _elem[lo];                          //大于pivot者归入右侧子序列
    }
    _elem[lo = pivot]; //将备份的轴点记录置于前/后子向量之间
    return lo;         //返回轴点的秩
}

template <typename T> Rank Vector<T>::partition_B(Rank lo, Rank hi) { //轴点构造算法(版本B)
    swap(_elem[lo], _elem[lo + rand() % (hi - lo + 1)]);              //任选一个元素与首元素交换
    T pivot = _elem[lo]; //以首元素为候选轴点: 经以上交换, 等效于随机选取
    while (lo < hi) {    //从向量对两端交替地向中间扫描
        while (lo < hi) {
            if (pivot < _elem[hi]) {     //在大于pivot的前提下
                hi--;                    //向左拓展右端子向量
            } else {                     //直至遇到不大于pivot者
                _elem[lo++] = _elem[hi]; //将其归入左端子向量
                break;
            }
        }
        while (lo < hi) {
            if (_elem[lo] < pivot) {     //在小于pivot的前提下
                lo++;                    //向右拓展左端子向量
            } else {                     //直至遇到不小于pivot者
                _elem[hi--] = _elem[lo]; //将其归入右端子向量
                break;
            }
        }
    }
    _elem[lo] = pivot; //将备份的轴点记录置于前/后子向量之间
    return lo;         //返回轴点的秩
}

template <typename T> Rank Vector<T>::partition_C(Rank lo, Rank hi) { //轴点构造算法(版本C)
    swap(_elem[lo], _elem[lo + rand() % (hi - lo + 1)]);              //任选一个元素与首元素交换
    T pivot = _elem[lo]; //以首元素为候选轴点: 经以上交换, 等效于随机选取
    int mi  = lo;
    for (int k = lo + 1; k <= hi; k++) { //自左向右扫描
        if (_elem[k] < pivot) {          //若当前元素_elem[k]小于pivot, 则
            swap(_elem[++mi], _elem[k]); //将_elem[k]交换至原mi之后, 使L子序列向右扩展
        }
    }
    swap(_elem[lo], _elem[mi]); //候选轴点归位
    return mi;                  //返回轴点的秩
}

template <typename T> Rank Vector<T>::partition(Rank lo, Rank hi) { //轴点构造算法
    // return partition_A(lo, hi); //轴点构造算法(版本A)
    // return partition_B(lo, hi); //轴点构造算法(版本B)
    return partition_C(lo, hi); //轴点构造算法(版本C)
}

template <typename T> void Vector<T>::quickSort(Rank lo, Rank hi) { //向量快速排序算法
    if (hi - lo < 2) return;                                        //单元素区间自然有序, 否则...
    Rank mi = partition(lo, hi - 1);                                //在[lo, hi-1]内构造轴点
    quickSort(lo, mi);                                              //对前缀递归排序
    quickSort(mi + 1, hi);                                          //对后缀递归排序
}

template <typename T> void Vector<T>::shellSort(Rank lo, Rank hi) { //向量希尔排序算法(神仙代码)
    for (int d = 0x3FFFFFFF; 0 < d; d >>= 1) { // PS Sequence: {1, 3, 7, 15, ..., 1073741823}
        for (int j = lo + d; j < hi; j++) {    //**到这儿就看不懂了: for each j in [lo+d, hi)
            T x   = _elem[j];
            int i = j - d;
            while (lo <= i && _elem[i] > x) {
                _elem[i + d] = _elem[i];
                i -= d;
            }
            _elem[i + d] = x;
        }
    }
}

template <typename T> void Vector<T>::sort(Rank lo, Rank hi) { //向量排序算法
    // return bubbleSort(lo, hi); //起泡排序算法
    // return mergeSort(lo, hi);     //归并排序算法
    // return selectionSort(lo, hi); //选择排序算法
    // return insertionSort(lo, hi); //插入排序算法
    // return heapSort(lo, hi); //堆排序算法
    return quickSort(lo, hi); //快速排序算法
    // return shellSort(lo, hi); //希尔排序算法
}

#endif