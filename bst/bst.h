#include "../bintree/binTree.h"

#ifndef BST_H
#define BST_H

template <typename T> class BST : public BinTree<T> { //由BinTree派生BST模版类
  protected:
    BinNodePosi(T) _hot;                                //"命中"节点的父亲
    BinNodePosi(T) connect34(                           //按照 "3 + 4" 结构, 联接3个节点及4棵子树
        BinNodePosi(T), BinNodePosi(T), BinNodePosi(T), // 3个节点
        BinNodePosi(T), BinNodePosi(T), BinNodePosi(T), BinNodePosi(T)); // 4棵子树
    BinNodePosi(T) rotateAt(BinNodePosi(T) x); //对x及其父亲, 祖父做统一旋转调整
    void stretchByZag(BinNodePosi(T) & x);     //通过zAg旋转调整, 将子树x拉伸成最左侧通路
    void stretchByZig(BinNodePosi(T) & x);     //通过zIg旋转调整, 将子树x拉伸成最右侧通路

  public:
    virtual BinNodePosi(T) & search(const T& e); //查找
    virtual BinNodePosi(T) insert(const T& e);   //插入
    virtual bool remove(const T& e);             //删除
    // DSA
    void stretchToLPath() { stretchByZag(_root); } //借助zag旋转, 转化为左向单链
    void stretchToRPath() { stretchByZig(_root); } //借助zig旋转, 转化为右向单链
};

template <typename T> //在以v为根的(AVL, SPLAY, rbTree等)BST子树中查找关键码e
    static BinNodePosi(T) & searchIn(BinNodePosi(T) & v, const T& e, BinNodePosi(T) & hot) {
    if (!v || (e == v->data)) return v; //递归基: 在节点v(或假想的通配节点)处命中
    hot = v;                            //一般情况, 先记下当前节点, 然后再
    return searchIn(((e < v->data) ? v->lc : v->rc), e, hot); //深入一层, 递归查找
}

template <typename T> BinNodePosi(T) & BST<T>::search(const T& e) { //在BST中查找关键码e
    return searchIn(_root, e, _hot = nullptr); //返回咪表节点位置的引用, 以便后序插入/删除操作
}

template <typename T> BinNodePosi(T) BST<T>::insert(const T& e) { //将关键码e插入BST树中
    BinNodePosi(T)& x = search(e);
    if (x) return x;             //确认目标不存在
    x = new BinNode<T>(e, _hot); //创建新节点x: 以e为关键码, 以_hot为父
    _size++;                     //更新全树规模
    updateHeightAbove(x);        //更新x及其历代祖先的高度
    return x;                    //新插入的节点, 必为叶子
}

// BST节点删除算法: 删除位置x所指的节点(全局静态模版函数, 适用于AVL, Splay, RedBlack等各种BST)
//目标x在此前经查找定位, 并确认非NULL, 故必删除成功; 与search不同, 调用之前不必将hot置空
//返回值指向实际被删除节点的接替者, hot指向实际被删除节点的父亲. (两者均有可能是NULL)
template <typename T> static BinNodePosi(T) removeAt(BinNodePosi(T) & x, BinNodePosi(T) & hot) {
    BinNodePosi(T) w    = x;       //实际被摘除的节点, 初值同x
    BinNodePosi(T) succ = nullptr; //实际被删除节点的接替者
    if (!HasLChild(*x))            //若*x的左子树为空, 则可
        succ = x = x->rc;          //直接将*x替换为其右子树
    else if (!HasRChild(*x))       //若*x右子树为空, 则可
        succ = x = x->lc;          //直接将*x替换为其左子树
    else { //若左右子树都存在, 则选择x的直接后继作为实际被摘除节点, 为此需要
        w = w->succ();          //(在右子树中)找到*x的直接后继*w
        swap(x->data, w->data); //交换*x和*w的数据元素
        BinNodePosi(T) u           = w->parent;
        ((u == x) ? u->rc : u->lc) = succ = w->rc; //隔离节点*w
    }
    hot = w->parent;              //实际被删除节点的父亲
    if (succ) succ->parent = hot; //被删除节点的接替者与hot相联
    if (w->data) delete w->data;  // release(w->data)
    if (w) delete w;              // release(w)
    return succ;
}

template <typename T> bool BST<T>::remove(const T& e) { //从BST树中删除关键码e
    BinNodePosi(T)& x = search(e);
    if (!x) return false;    //确认目标存在(留意_hot的设置)
    removeAt(x, _hot);       //实施删除
    _size--;                 //更新全树规模
    updateHeightAbove(_hot); //更新_hot及其历代祖先的高度(不从x更新, 因为x已经被删除了)
    return true;
}

template <typename T>
BinNodePosi(T) BST<T>::connect34( //按照 "3 + 4" 结构, 联接3个节点及4棵子树
    BinNodePosi(T) a, BinNodePosi(T) b, BinNodePosi(T) c,                         // 3个节点
    BinNodePosi(T) T0, BinNodePosi(T) T1, BinNodePosi(T) T2, BinNodePosi(T) T3) { // 4棵子树
    a->lc = T0;
    if (T0) T0->parent = a;
    a->rc = T1;
    if (T1) T1->parent = a;
    updateHeight(a);
    c->lc = T2;
    if (T2) T2->parent = c;
    c->rc = T3;
    if (T3) T3->parent = c;
    updateHeight(c);
    b->lc     = a;
    a->parent = b;
    b->rc     = c;
    c->parent = b;
    updateHeight(b);
    return b; //返回根节点的位置
}

template <typename T> BinNodePosi(T) BST<T>::rotateAt(BinNodePosi(T) v) { // v为非空孙辈节点
    BinNodePosi(T) p = v->parent; //视v, p和g相对位置分四种情况
    BinNodePosi(T) g = p->parent;
    if (IsLChild(*p)) {     // zIg
        if (IsLChild(*v)) { // zIg-zIg
            p->parent = g->parent;
            return connect34(v, p, g, v->lc, v->rc, p->rc, g->rc);
        } else { // zIg-zAg
            v->parent = g->parent;
            return connect34(p, v, g, p->lc, v->lc, v->rc, g->rc);
        }
    } else {                // zAg
        if (IsRChild(*v)) { // zAg-zAg
            p->parent = g->parent;
            return connect34(g, p, v, g->lc, p->lc, v->lc, v->rc);
        } else { // zAg-zIg
            v->parent = g->parent;
            return connect34(g, v, p, g->lc, v->lc, v->rc, p->rc);
        }
    }

    return nullptr
}

template <typename T> void stretchByZag(BinNodePosi(T) & x) { //通过zAg旋转调整, 将子树x拉伸成最左侧通路
    BinNodePosi(T) p = x;
    while (p->rc) p = p->rc;        //最大节点, 必须是子树最终的根
    while (x->lc) x = x->lc;        //转至最左侧通路的末端
    for (; x != p; x = x->parent) { //若右子树已空, 则上升一层
        while (x->rc)               //否则, 反复地
            x->zag();               //以x为轴做zAg操作
    }                               //直至抵达子树的根
}

template <typename T> void stretchByZig(BinNodePosi(T) & x) { //通过zIg旋转调整, 将子树x拉伸成最右侧通路
    BinNodePosi(T) v = x;
    while (v->lc) v = v->lc;        //最大节点, 必须是子树最终的根
    while (x->rc) x = x->rc;        //转至最右侧通路的末端
    for (; x != v; x = x->parent) { //若左子树已空, 则上升一层
        while (x->lc)               //否则, 反复地
            x->zig();               //以x为轴左zIg操作
    }                               //直至抵达子树的根
}

#endif