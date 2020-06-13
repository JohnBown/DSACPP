#include "../vector/vector.h"

#ifndef BTNODE_H
#define BTNODE_H

#define BTNodePosi(T) BTNode<T>* // B-树节点位置

template <typename T> struct BTNode { // B-树节点模版类
    //成员
    BTNodePosi(T) parent;        //父节点
    Vector<T> key;               //关键码向量
    Vector<BTNodePosi(T)> child; //孩子向量(长度总比key多一)
    //构造函数
    BTNode() {
        parent = nullptr;
        child.insert(0, nullptr);
    }
    BTNode(T e, BTNodePosi(T) lc = nullptr, BTNodePosi(T) rc = nullptr) {
        parent = nullptr;    //作为根节点, 而且初始时
        key.insert(0, e);    //只有一个关键码, 以及
        child.insert(0, lc); //两个孩子
        child.insert(0, rc);
        if (lc) lc->parent = this;
        if (rc) rc->parent = this;
    }
};

#endif