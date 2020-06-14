#include "../bst/bst.h"

#ifndef REDBLACK_H
#define REDBLACK_H

template <typename T> class RedBlack : public BST<T> { // RedBlack树模版类
  protected:
    void solveDoubleRed(BinNodePosi(T) x);   //双红修正
    void solveDoubleBlack(BinNodePosi(T) x); //双黑修正
    int updateHeight(BinNodePosi(T) x);      //更新节点x的高度

  public:
    BinNodePosi(T) insert(const T& e); //插入
    bool remove(const T& e);           //删除
    // BST::search()等其余接口可直接沿用
};

template <typename T> int RedBlack<T>::updateHeight(BinNodePosi(T) x) { //更新节点高度
    x->height = __max(stature(x->lc), stature(x->rc)); //孩子一般黑高度相等, 除非出现双黑
    return IsBlack(x) ? x->height++ : x->height;       //若当前节点为黑, 则计入黑深度
}

template <typename T> BinNodePosi(T) RedBlack<T>::insert(const T& e) { //将e插入红黑树
    BinNodePosi(T)& x = search(e); //确认目标节点不存在(留意对_hot的设置)
    if (x) return x;
    x = new BinNode<T>(e, this->_hot, nullptr, nullptr, -1); //创建红节点x: 以_hot为父, 黑高度-1
    this->_size++;
    solveDoubleRed(x);                 //经双红修正后
    return x ? x : this->_hot->parent; //即可返回
}

template <typename T> bool RedBlack<T>::remove(const T& e) { //从红黑树中删除关键码e
    BinNodePosi(T)& x = search(e); //确认目标节点已存在(留意_hot的设置)
    if (!x) return false;
    BinNodePosi(T) r = removeAt(x, this->_hot); //实施删除
    if (!(--this->_size)) return true;
    if (!this->_hot) { //若刚被删除的是根节点, 则将其置黑, 并更新黑高度
        this->_root->color = RB_BLACK;
        updateHeight(this->_root);
        return true;
    }
    if (BlackHeightUpdated(*this->_hot)) return true; //若所有祖先的黑深度依然平衡, 则无需调整
    if (IsRed(r)) {                             //否则, 若r为红, 则只需令其转黑
        r->color = RB_BLACK;
        r->height++;
        return true;
    }
    solveDoubleBlack(r); //经双黑调整
    return true;         //即可返回
}

//双红修正算法: 解决节点x与其父均为红色的问题, 分为两种情况:
// RR-1: 2次颜色翻转, 2次黑高度更新, 1~2次旋转, 不再递归
// RR-2: 3次颜色翻转, 3次黑高度更新, 0次旋转, 需要递归
template <typename T> void RedBlack<T>::solveDoubleRed(BinNodePosi(T) x) { // x当前必为红
    if (IsRoot(*x)) {                                                      //若已(递归)转至树根
        this->_root->color = RB_BLACK;                                           //则将其转黑
        this->_root->height++;                                                   //整树黑高度也随之递增
        return;
    }
    BinNodePosi(T) p = x->parent;           //否则, x的父亲p必存在
    if (IsBlack(p)) return;                 //若p为黑, 则可终止调整(不会发生双红冲突). 否则
    BinNodePosi(T) g = p->parent;           //既然p为红, 则x的祖父必存在, 且必为黑色
    BinNodePosi(T) u = uncle(x);            //一下, 视x的叔父u的颜色分别处理
    if (IsBlack(u)) {                       //(情况一: u为黑色时)
        if (IsLChild(*x) == IsLChild(*p)) { //若x与p同侧(即zIg-zIg或zAg-zAg), 则
            p->color = RB_BLACK;            // p由红转黑, x保持红
        } else {                            //若x与p异侧(即zIg-zAg或zAg-zIg), 则
            x->color = RB_BLACK;            // x由红转黑, p保持红
        }
        g->color          = RB_RED;                        // g必定由黑转红
        BinNodePosi(T) gg = g->parent;                     //曾祖父(great-grand parent)
        BinNodePosi(T) r = FromParentTo(*g) = rotateAt(x); //调整后的子树根节点
        r->parent                           = gg;          //与原曾祖父联接
    } else {                                               //(情况二: u为红色时)
        p->color = RB_BLACK;                               // p由红转黑
        p->height++;
        u->color = RB_BLACK; // u由红转黑
        u->height++;
        if (!IsRoot(*g)) g->color = RB_RED; // g若非根, 则转红
        solveDoubleRed(g);                  //继续调整g(类似于尾递归, 可优化为迭代形式)
    }
}

//双黑修正算法: 解决节点x与被其替代的节点均为黑色的问题, 分为三大类共四种情况:
// BB-1: 2次颜色翻转, 2次黑高度更新, 1~2次旋转, 不再递归
// BB-2R: 2次颜色翻转, 2次黑高度更新, 0次旋转, 不再递归
// BB-2B: 1次颜色翻转, 1次黑高度更新, 0次旋转, 需要递归
// BB-3: 2次颜色翻转, 2次黑高度更新, 1次旋转, 转为BB-1或BB-2R
template <typename T> void RedBlack<T>::solveDoubleBlack(BinNodePosi(T) r) {
    BinNodePosi(T) p = r ? r->parent : this->_hot; // r的父亲
    if (!p) return;
    BinNodePosi(T) s = (r == p->lc) ? p->rc : p->lc; // r的兄弟
    if (IsBlack(s)) {                                //兄弟s为黑
        BinNodePosi(T) t = nullptr; // s的红孩子(若左/右孩子皆红, 左者优先; 皆黑时为nullptr)
        if (IsRed(s->rc)) t = s->rc;     //右子
        if (IsRed(s->lc)) t = s->lc;     //左子
        if (t) {                         //(情况一: 黑s有红孩子 BB-1)
            RBColor oldColor = p->color; //备份原子树根节点p颜色, 并对t及其父亲, 祖父
            //以下, 通过旋转重平衡, 并将新子树的左, 右孩子染黑
            BinNodePosi(T) b = FromParentTo(*p) = rotateAt(t); //旋转
            if (HasLChild(*b)) {                               //左子
                b->lc->color = RB_BLACK;
                updateHeight(b->lc);
            }
            if (HasRChild(*b)) { //右子
                b->rc->color = RB_BLACK;
                updateHeight(b->rc);
            }
            b->color = oldColor; //新子树根节点继承原根节点的颜色
            updateHeight(b);
        } else {               //黑s无红孩子
            s->color = RB_RED; // s转红
            s->height--;
            if (IsRed(p)) {          //(情况二: 黑s无红孩子, p为红 BB-2R)
                p->color = RB_BLACK; // p转黑, 但黑高度不变
            } else {                 //(情况三: 黑s无红孩子, p为黑 BB-2B)
                p->height--;         // p保持, 但黑高度下降
                solveDoubleBlack(p); //递归上溯, 下溢提升
            }
        }
    } else {                                             //(情况四: 兄弟s为红 BB-3)
        s->color         = RB_BLACK;                     // s转黑
        p->color         = RB_RED;                       // p转红
        BinNodePosi(T) t = IsLChild(*s) ? s->lc : s->rc; //取t与其父s同侧
        this->_hot             = p;
        FromParentTo(*p) = rotateAt(t); //对t及其父亲, 祖父做平衡调整
        solveDoubleBlack(r); //继续修正r处双黑(此时p已转红, 故后续只能是BB-1或BB-2R)
    }
}

#endif