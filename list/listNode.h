#ifndef LISTNODE_H
#define LISTNODE_H

typedef int Rank;                     //秩
#define ListNodePosi(T) ListNode<T> * //列表节点位置

template <typename T> struct ListNode {
    //成员
    T data;               //数值
    ListNodePosi(T) pred; //前驱
    ListNodePosi(T) succ; //后继
    //构造函数
    ListNode() {} //针对header和trailer的构造
    ListNode(T e, ListNodePosi(T) p = NULL,
             ListNodePosi(T) s = NULL) //默认构造器
        : data(e), pred(p), succ(s) {}
    //操作接口
    ListNodePosi(T) insertAsPred(T const &e); //靠近当前节点之前插入新节点
    ListNodePosi(T) insertAsSucc(T const &e); //靠近当前节点之后插入新节点
};

template <typename T>
ListNodePosi(T) ListNode<T>::insertAsPred(
    T const &e) { //将e紧靠当前节点之前，插入当前节点所属列表（设有哨兵头节点header）
    ListNodePosi(T) x = new ListNode(e, pred, this); //创建新节点
    pred->succ = x;                                  //设置正向链接
    pred = x;                                        //设置正向链接
    return x;                                        //返回新节点的位置
}

template <typename T>
ListNodePosi(T) ListNode<T>::insertAsSucc(
    T const &e) { //将e紧靠当前节点之后，插入当前节点所属列表（设有哨兵尾节点trailer）
    ListNodePosi(T) x = new ListNode(e, this, succ); //创建新节点
    succ->pred = x;                                  //设置正向链接
    succ = x;                                        //设置正向链接
    return x;                                        //返回新节点的位置
}

#endif