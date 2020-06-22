#include "../entry/entry.h"

#ifndef QUADLISTNODE_H
#define QUADLISTNODE_H

typedef int Rank;                         //秩
#define QlistNodePosi(T) QuadlistNode<T>* //跳转表节点位置

template <typename T> struct QuadlistNode { // QuadlistNode模版类
    T entry;                                //所存词条
    QlistNodePosi(T) pred;                  //前驱
    QlistNodePosi(T) succ;                  //后继
    QlistNodePosi(T) above;                 //上邻
    QlistNodePosi(T) below;                 //下邻

    QuadlistNode(T e = T(), QlistNodePosi(T) p = nullptr, QlistNodePosi(T) s = nullptr,
                 QlistNodePosi(T) a = nullptr, QlistNodePosi(T) b = nullptr) :
        entry(e), pred(p), succ(s), above(a), below(b) {} //构造器

    QlistNodePosi(T) //插入新节点, 以当前节点为前驱, 以节点b为下邻
        insertAsSuccAbove(T const& e, QlistNodePosi(T) b = nullptr);
};

template <typename T> //将e作为当前节点的后继, b的上邻插入Quadlist
QlistNodePosi(T) QuadlistNode<T>::insertAsSuccAbove(T const& e, QlistNodePosi(T) b) {
    QlistNodePosi(T) x = new QuadlistNode<T>(e, this, succ, nullptr, b); //创建新节点
    succ->pred         = x;
    succ               = x; //设置水平逆向链接
    if (b) b->above = x;    //设置垂直逆向链接
    return x;               //返回新节点的位置
}

#endif