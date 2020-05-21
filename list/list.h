#ifndef LIST_H
#define LIST_H

#include "listNode.h"

template <typename T> class List {
  private:
    int _size;               //规模
    ListNodePosi(T) header;  //头哨兵
    ListNodePosi(T) trailer; //尾哨兵

  protected:
    void init();
    int clear();
    void copyNode(ListNodePosi(T), int);                                   //复制列表中自位置p起n项
    void merge(ListNodePosi(T) &, int, List<T> &, ListNodePosi(T) &, int); //归并
    void mergeSort(ListNodePosi(T) &, int);   //对从p开始连续n个节点归并排序
    void selectionSort(ListNodePosi(T), int); //对从p开始连续n个节点选择排序
    void insertionSort(ListNodePosi(T), int); //对从p开始连续n个节点插入排序

  public:
    //构造函数
    List() { init(); }                     //默认
    List(List<T> const &L);                //整体复制列表L
    List(List<T> const &L, Rank r, int n); //复制列表L中自r项起的n项
    List(ListNodePosi(T) p, int n);        //复制列表中自位置p起n项
    //析构函数
    ~List(); //释放(包含头、尾哨兵在内的)所有节点
    //只读访问接口
    Rank size() const { return _size; }                    //规模
    bool empty() const { return _size <= 0; }              //判空
    T &operator[](Rank r) const;                           //重载, 支持寻秩访问(效率低)
    ListNodePosi(T) first() const { return header->succ; } //首节点位置
    ListNodePosi(T) last() const { return trailer->pred; } //末节点位置
    ListNodePosi(T) find(T const &e) const { return find(e, _size, trailer); }     //无序列表查找
    ListNodePosi(T) find(T const &e, int n, ListNodePosi(T) p) const;              //无序区间查找
    ListNodePosi(T) search(T const &e) const { return search(e, _size, trailer); } //有序列表查找
    ListNodePosi(T) search(T const &e, int n, ListNodePosi(T) p) const;            //有序列表查找
    ListNodePosi(T) selectMax(ListNodePosi(T) p, int n); //在p及其n-1个后继中选出最大者
    ListNodePosi(T) selectMax() { return selectMax(header->succ, _size); }; //整体最大者
    //可写访问接口
    ListNodePosi(T) insertAsFirst(T const &e);              //将e当作首节点插入
    ListNodePosi(T) insertAsLast(T const &e);               //将e当作末节点插入
    ListNodePosi(T) insertA(ListNodePosi(T) p, T const &e); //将e当作p的后继插入(After)
    ListNodePosi(T) insertB(ListNodePosi(T) p, T const &e); //将e当作p的前驱插入(Before)
    T remove(ListNodePosi(T) p);                            //删除合法节点p, 返回其数值
    void sort(ListNodePosi(T) p, int n);                    //列表区间排序
    void sort() { sort(first(), _size); }                     //列表整体排序
    int deduplicate();                                      //无序去重
    int uniquify();                                         //有序去重
    void reverse();                                         //前后倒置
};

template <typename T> void List<T>::init() { //列表初始化, 列表创建时调用
    header = new ListNode<T>;                //创建头哨兵节点
    trailer = new ListNode<T>;               //创建尾哨兵节点
    header->succ = trailer, header->pred = nullptr;
    trailer->pred = header, trailer->succ = nullptr;
    _size = 0; //记录规模
}

template <typename T> int List<T>::clear() { //清空列表
    int oldSize = _size;
    while (0 < _size) { //反复删除首节点, 直至列表变空
        remove(header->succ);
    }
    return oldSize;
}

template <typename T> void List<T>::copyNode(ListNodePosi(T) p, int n) { // p合法, 且至少有n-1个真后继节点
    init(); //创建头、尾哨兵节点并做初始化
    while (n--) {
        insertAsLast(p->data); //自p起的n项依次作为末节点插入
        p = p->succ;
    }
}

template <typename T> List<T>::List(ListNodePosi(T) p, int n) { //复制列表L自p起n项
    copyNode(p, n);
}

template <typename T> List<T>::List(List<T> const &L) { //整体复制列表L
    copyNode(L.first, L._size);
}

template <typename T> List<T>::List(List<T> const &L, Rank r, int n) { //复制列表L自r项起的n项
    copyNode(L[r], n);
}

template <typename T> List<T>::~List() { //列表析构函数
    clear();                             //清空列表
    delete header;                       //释放头哨兵节点
    delete trailer;                      //释放尾哨兵节点
}

template <typename T> T &List<T>::operator[](Rank r) const { //重载, 低效慎用
    ListNodePosi(T) p = first();                             //首节点出发
    while (0 < r--) {                                        //顺数第r个节点
        p = p->succ;
    }
    return p->data; //目标节点, 返回其中所存元素
}

template <typename T> //在无序列表内节点p(可能是trailer)的n个前驱中, 找到等于e的最后者
ListNodePosi(T) List<T>::find(T const &e, int n, ListNodePosi(T) p) const {
    while (0 < n--) {                 // p的n个前驱, 从右向左
        if (e == (p = p->pred)->data) //逐个比对, 直至命中或范围越界
            return p;
    }
    return nullptr; // p越出左界意味着区间内不含e, 查找失败
}

template <typename T> //有序列表哪你节点p(可能是trailer)对n个(真)前驱中, 找到不大于e的最后者
ListNodePosi(T) List<T>::search(T const &e, int n, ListNodePosi(T) p) const {
    while (0 <= n--) {                 //对于p的最近的n个前驱, 从右向左逐个比较
        if ((p = p->pred)->data <= e) //直至命中、数值越界或范围哦越界
            break;
    }
    return p; //返回查找终止的位置
}

template <typename T> ListNodePosi(T) List<T>::insertAsFirst(T const &e) { //将e当作首节点插入
    _size++;
    return header->insertAsSucc(e);
}

template <typename T> ListNodePosi(T) List<T>::insertAsLast(T const &e) { //将e当作尾节点插入
    _size++;
    return trailer->insertAsPred(e);
}

template <typename T>
ListNodePosi(T) List<T>::insertA(ListNodePosi(T) p, T const &e) { //将e当作p的后继插入(After)
    _size++;
    return p->insertAsSucc(e);
}

template <typename T>
ListNodePosi(T) List<T>::insertB(ListNodePosi(T) p, T const &e) { //将e当作p的前驱插入(Before)
    _size++;
    return p->insertAsPred(e);
}

template <typename T> T List<T>::remove(ListNodePosi(T) p) { //删除节点p, 返回值
    T e = p->data;           //备份待删除节点的数值(假定T类型可直接赋值)
    p->pred->succ = p->succ; //后继
    p->succ->pred = p->pred; //前驱
    delete p;                //释放节点
    _size--;                 //更新规模
    return e;                //返回备份数据
}

template <typename T>
ListNodePosi(T) List<T>::selectMax(ListNodePosi(T) p, int n) { //从起始于位置p的n个元素中选出最大者
    ListNodePosi(T) max = p;                                   //最大者暂定为首节点p
    for (ListNodePosi(T) cur = p; 1 < n; n--) { //从首节点p出发, 将后续节点逐一与max比较
        cur = cur->succ;
        if (max->data <= cur->data) //若当前元素不小于max,则
            max = cur;              //更新最大元素位置记录
    }
    return max; //返回最大节点位置
}

template <typename T> void List<T>::selectionSort(ListNodePosi(T) p, int n) { //列表区间[p, p + n)选择排序算法
    ListNodePosi(T) head = p->pred;                                           //区间首
    ListNodePosi(T) tail = p;                                                 //区间末
    for (int i = 0; i < n; i++) {                                             //待排序区间为(head, tail)
        tail = tail->succ;
    }
    while (1 < n) {                               //在至少还剩两个节点之前, 在待排序区间内
        ListNodePosi(T) max = selectMax(head, n); //找出最大者(歧义时后者优先)
        insertB(tail, remove(max)); //将其移至无序区间末尾(作为有序区间新的首元素)
        tail = tail->pred;          //更新无序区间末尾
        n--;                        //更新待排序区间长度
    }
}

template <typename T> void List<T>::insertionSort(ListNodePosi(T) p, int n) { //列表区间[p, p + n)插入排序算法
    for (int r = 0; r < n; r++) {                                             //逐一为各节点
        insertA(search(p->data, r, p), p->data);                              //查找适合的位置并插入
        p = p->succ;                                                          //转向下一节点
        remove(p->pred);                                                      //移除上一节点
    }
}

template <typename T> //有序列表的归并：当前列表中自p起n个元素, 与当前列表中自q起m个元素
void List<T>::merge(ListNodePosi(T) & p, int n, List<T> &L, ListNodePosi(T) & q, int m) {
    ListNodePosi(T) pp = p->pred;              //借助p的前驱, 方便最后找回p
    while (0 < m) {                            //在q尚未移出区间之前
        if ((0 < n) && (p->data <= q->data)) { // p也未移出区间, 且L[p] <= L[q]
            if (q == (p = p->succ))            // p归入合并列表, 并替换为其直接后继
                break;
            n--;
        } else {                                       //若已超出右界或L[q] < L[p]
            insertB(p, L.remove((q = q->succ)->pred)); //将q转移至p之前
            m--;
        }
    }
    p = pp->succ; //确定归并后区间的(新)起点
}

template <typename T> void List<T>::mergeSort(ListNodePosi(T) & p, int n) { //列表归并排序算法
    if (n < 2) //若待排序范围足够小, 则直接返回
        return;
    int m = n >> 1;        //以中心为界
    ListNodePosi(T) q = p; //均分列表
    for (int i = 0; i < m; i++) {
        q = q->succ;
    }
    mergeSort(p, m);              //对前子序列归并排序
    mergeSort(q, n - m);          //对后子序列归并排序
    merge(p, m, *this, q, n - m); //归并
} //排序后, p依然指向归并后区间的(新)起点

template <typename T> void List<T>::sort(ListNodePosi(T) p, int n) { //列表区间排序
    return selectionSort(p, n);
    // return insertionSort(p, n);s
    // return mergeSort(p, n);
}

template <typename T> int List<T>::deduplicate() { //剔除无序列表中的重复节点
    if (_size < 2)                                 //平凡列表自然无重复
        return 0;
    int oldSize = _size;                         //记录原规模
    ListNodePosi(T) p = header;                  // p从首节点开始
    Rank r = 0;                                  // r从0计数
    while (trailer != (p->succ)) {               //依次知道末节点
        ListNodePosi(T) q = find(p->data, r, p); //在p的r个真前驱中查找雷同者
        q ? remove(q) : r++;                     //若的确存在, 则删除之；否则秩加一
    }                       // assert: 循环过程中的任意时刻, p的所有前驱互不相同
    return oldSize - _size; //列表规模变化量, 既被删除元素总数
}

template <typename T> int List<T>::uniquify() { //成批剔除重复元素, 效率更高
    if (_size < 2)                              //平凡列表自然无重复
        return 0;
    int oldSize = _size;               //记录原规模
    ListNodePosi(T) p = first();       // p为各区段起点
    ListNodePosi(T) q;                 // q为p后继
    while (trailer != (q = p->succ)) { //反复考察紧邻节点对(p, q)
        if (p->data != q->data) {      //若互异, 则转向下一区段
            p = q;
        } else { //否则(雷同), 删除后者
            remove(q);
        }
    }
    return oldSize - _size; //列表规模变化量, 既被删除元素总数
}

#include "../vector/vector.h"
template <typename T> void List<T>::reverse() { //前后倒置. 共计三种实现方式, 采用了较复杂的其一
    if (_size < 2)                              //平凡情况
        return;
    ListNodePosi(T) p;
    ListNodePosi(T) q;
    for (p = header, q = p->succ; p != trailer; p = q, q = p->succ) {
        p->pred = q; //自前向后, 依次颠倒各节点的前驱指针
    }
    trailer->pred = nullptr; //单独设置尾节点的前驱指针
    for (p = header, q = p->pred; p != trailer; p = q, q = p->pred) {
        q->succ = p; //自前向后, 依次颠倒各节点的后继指针
    }
    header->succ = nullptr;   //单独设置头节点的后继指针
    swap(header, trailer); //头、尾节点互换
}

#endif