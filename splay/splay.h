#include "../bst/bst.h"

#ifndef SPLAY_H
#define SPLAY_H

template <typename T> class Splay : public BST<T> {
  protected:
    BinNodePosi(T) splay(BinNodePosi(T) v);

  public:
    BinNodePosi(T) & search(const T& e);
    BinNodePosi(T) insert(const T& e);
    bool remove(const T& e);
};

template <typename NodePosi>
inline void attachAsLChild(NodePosi p, NodePosi lc) { //在节点*p与*lc(可能为空)之间建立父(左)子关系
    p->lc = lc;
    if (lc) lc->parent = p;
}

template <typename NodePosi>
inline void attachAsRChild(NodePosi p, NodePosi rc) { //在节点*p与*rc(可能为空)之间建立父(右)子关系
    p->rc = rc;
    if (rc) rc->parent = p;
}

template <typename T>
BinNodePosi(T) Splay<T>::splay(BinNodePosi(T) v) { // Splay树伸展算法: 从节点v出发逐层伸展
    if (!v) return nullptr;
    BinNodePosi(T) p;                            //*v的父亲
    BinNodePosi(T) g;                            //*v的祖父
    while ((p = v->parent) && (g = p->parent)) { //自下而上, 反复对*v做双层伸展
        BinNodePosi(T) gg = g->parent;           //每轮之后*v都以原曾祖父为父
        if (IsLChild(*v)) {                      // zIg
            if (IsLChild(*p)) {                  // zIg-zIg
                attachAsLChild(g, p->rc);
                attachAsLChild(p, v->rc);
                attachAsRChild(p, g);
                attachAsRChild(v, p);
            } else { // zIg-zAg
                attachAsLChild(p, v->rc);
                attachAsRChild(g, v->lc);
                attachAsLChild(v, g);
                attachAsRChild(v, p);
            }
        } else {                // zAg
            if (IsRChild(*p)) { // zAg-zAg
                attachAsRChild(g, p->lc);
                attachAsRChild(p, v->lc);
                attachAsLChild(p, g);
                attachAsLChild(v, p);
            } else { // zAg-zIg
                attachAsRChild(p, v->lc);
                attachAsLChild(g, v->rc);
                attachAsRChild(v, g);
                attachAsLChild(v, p);
            }
        }
        if (!gg) { //若*v原先的曾祖父*gg不存在, 则*v现在应为树根
            v->parent = nullptr;
        } else { //否则, *gg此后应该以*v作为左或右孩子
            (g == gg->lc) ? attachAsLChild(gg, v) : attachAsRChild(gg, v);
        }
        updateHeight(g);
        updateHeight(p);
        updateHeight(v);
    } //双层伸展结束时, 必有g == nullptr, 但p可能非空

    if (p = v->parent) { //若p果真非空, 则额外再做一次单旋
        if (IsLChild(*v)) {
            attachAsLChild(p, v->rc);
            attachAsRChild(v, p);
        } else {
            attachAsRChild(p, v->lc);
            attachAsLChild(v, p);
        }
        updateHeight(p);
        updateHeight(v);
    }
    v->parent = nullptr;
    return v;
} //调整之后新树根应为被伸展的节点, 故返回该节点的位置以便上层函数更新树根

template <typename T> BinNodePosi(T) & Splay<T>::search(const T& e) { //在伸展树中查找e
    BinNodePosi(T) p = searchIn(this->_root, e, this->_hot = nullptr);
    this->_root      = splay(p ? p : this->_hot); //将最后一个被访问的节点伸展至根
    return this->_root;
} //与其他BST不同, 无论查找成功与否, _root都指向最后被访问的节点

template <typename T> BinNodePosi(T) Splay<T>::insert(const T& e) { //将关键码e插入伸展树中
    if (!this->_root) {                                             //处理原树为空的退化情况
        this->_size++;
        return this->_root = new BinNode<T>(e);
    }
    if (e == search(e)->data) return this->_root; //确认目标节点不存在
    this->_size++;
    BinNodePosi(T) t = this->_root; //创建新节点. 一下调整<=7个指针以完成局部重构
    if (this->_root->data < e) {    //插入新根, 以t和t->rc为左/右孩子
        t->parent = this->_root = new BinNode<T>(e, nullptr, t, t->rc);
        if (HasRChild(*t)) {
            t->rc->parent = this->_root;
            t->rc         = nullptr;
        }
    } else { //插入新根, 以t->lc和t为左/右孩子
        t->parent = this->_root = new BinNode<T>(e, nullptr, t->lc, t);
        if (HasLChild(*t)) {
            t->lc->parent = this->_root;
            t->lc         = nullptr;
        }
    }
    updateHeightAbove(t); //更新t及其祖先(实际上只有_root一个)的高度
    return this->_root;   //新节点必然置于树根, 返回之
} //无论e是否存在于原树中, 返回时总有_root->data == e

template <typename T> bool Splay<T>::remove(const T& e) {     //从伸展树中删除关键码e
    if (!this->_root || (e != search(e)->data)) return false; //若树空或目标不存在, 则无法删除
    BinNodePosi(T) w = this->_root;                           //经过search()后节点e已被伸展至树根
    if (!HasLChild(*this->_root)) {                           //若无左子树
        this->_root = this->_root->rc;
        if (this->_root) this->_root->parent = nullptr;
    } else if (!HasRChild(*this->_root)) { //若无右子树
        this->_root = this->_root->lc;
        if (this->_root) this->_root->parent = nullptr;
    } else { //若左右子树同时存在
        BinNodePosi(T) lTree = this->_root->lc;
        lTree->parent        = nullptr;
        this->_root->lc      = nullptr; //暂时将左子树切除
        this->_root          = this->_root->rc;
        this->_root->parent  = nullptr; //至保留右子树
        search(w->data);                //以原树根为目标, 做一次(必定失败的)查找
        //至此, 右子树中最小节点必伸展至根, 且(因无雷同节点)其左子树必空, 于是
        this->_root->lc = lTree;
        lTree->parent   = this->_root; //只需将原左子树接回原位即可
    }
    if (w->data) delete w->data;                // release(w->data)
    if (w) delete w;                            // release(w)
    if (this->_root) updateHeight(this->_root); //此后, 若树非空, 则树根的高度需要更新
    return true;                                //返回成功标志
} //若目标节点存在且被删除, 返回true, 否则返回false

#endif