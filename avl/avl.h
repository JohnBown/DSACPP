#include "../bst/bst.h"

#ifndef AVL_H
#define AVL_H

#define Balanced(x) (stature((x).lc) == stature((x).rc))
#define BalFac(x) (stature((x).lc) - stature((x).rc))
#define AvlBalanced(x) ((-2 < BalFac(x)) && (BalFac(x) < 2))

//在左右孩子中取更高者, 左右孩子等高, 取与父亲同侧者(zIg-zIg或zAg-zAg)
#define tallerChild(x)                                          \
    (stature((x)->lc) > stature((x)->rc) ? (x)->rc : (x)->lc    \
     : (stature((x)->lc) < stature((x)->rc) ? (x)->rc : (x)->rc \
        : (IsLChild(*(x)) ? (x)->lc : (x)->rc)))

template <typename T> class AVL : public BST<T> { //由BST派生AVL树模版类
  public:
    BinNodePosi(T) insert(const T& e); //插入(重写)
    bool remove(const T& e);           //删除(重写)
    // BST::search()等其余接口可直接沿用
};

template <typename T> BinNodePosi(T) AVL<T>::insert(const T& e) { //关键码e插入AVL树中
    BinNodePosi(T)& x = search(e);
    if (x) return x; //确定目标节点不存在
    BinNodePosi(T) xx = x = new BinNode<T>(e, _hot);
    _size++; //创建新节点
    //此时, x的父亲_hot若增高, 则其祖父有可能失衡
    for (BinNodePosi(T) g = _hot; g; g = g->parent) { //从x之父出发向上, 逐层检查各代祖先g
        if (!AvlBalanced(*g)) { //一旦发现g失衡, 则(采用"3+4"算法)使之复衡, 并将子树
            FromParentTo(*g) = rotateAt(tallerChild(tallerChild(g))); //重新接入原树
            break; // g复衡后, 局部子树高度必然复原; 其祖先亦必如此, 故调整随即结束
        } else {   //否则(g依然平衡), 只需简单地
            updateHeight(g); //更新其高度(注意: 即便g未失衡, 高度亦可能增加)
        }
    }          //至多只需一次调整; 若果真做过调整, 则全树高度必然复原
    return xx; //返回新节点位置
} //无论e是否存在于原树中, 总有AVL::insert(e)->data == e

template <typename T> bool AVL<T>::remove(const T& e) { //从AVL树中删除关键码e
    BinNodePosi(T)& x = search(e);
    if (!x) return false; //确认目标存在
    removeAt(x, _hot);    //先按BST规则删除之(此后, 原节点之父_hot及其祖先均可能失衡)
    _size--;
    for (BinNodePosi(T) g = _hot; g; g = g->parent) { //从_hot出发向上, 逐层检查各代祖先g
        if (!AvlBalanced(*g)) //一旦发现g失衡, 则(采用"3+4"算法)使之复衡, 并将该子树联至
            g = FromParentTo(*g) = rotateAt(tallerChild(tallerChild(g))); //原父亲
        updateHeight(g); //并更新其高度(注意: 即便g未失衡, 高度亦可能降低)
    }            //可能需做Omega(logn)次调整(无论是否做过调整, 全树高度均可能降低)
    return true; //删除成功
} //若目标节点存在且被删除, 返回true; 否则返回false

#endif