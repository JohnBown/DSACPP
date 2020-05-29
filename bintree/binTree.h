#ifndef BIN_TREE_H
#define BIN_TREE_H

#include "binNode.h" //引入二叉树节点类

template <typename T> class BinTree { //二叉树模版类
  protected:
    int _size;                                  //规模
    BinNodePosi(T) _root;                       //根节点
    virtual int updateHeight(BinNodePosi(T) x); //更新节点x的高度
    void updateHeightAbove(BinNodePosi(T) x);   //更新节点x及其祖先的高度

  public:
    BinTree() : _size(0), _root(nullptr) {} //构造函数
    ~BinTree() {                            //析构函数
        if (0 < _size) remove(_root);
    }
    int size() const { return _size; }                       //规模
    bool empty() const { return !_root; }                    //判空
    BinNodePosi(T) root() { return _root; }                  //树根
    BinNodePosi(T) insertAsRoot(T const& e);                 //插入根节点
    BinNodePosi(T) insertAsLC(BinNodePosi(T) x, T const& e); // e作为x的左孩子插入(原无)
    BinNodePosi(T) insertAsRC(BinNodePosi(T) x, T const& e); // e作为x的右孩子插入(原无)
    BinNodePosi(T) attachAsLC(BinNodePosi(T) x, BinTree<T>*& S); // T作为x左子树接入
    BinNodePosi(T) attachAsRC(BinNodePosi(T) x, BinTree<T>*& S); // T作为x右子树接入
    int remove(BinNodePosi(T) x); //删除以位置x出节点为根的子树, 返回该子树原先的规模
    BinTree<T>* secede(BinNodePosi(T) x); //将子树x从当前树中摘除, 并将其转化成一颗独立子树

    template <typename VST> void travLevel(VST& visit) { //层次遍历
        if (_root) _root->travLevel(visit);
    }
    template <typename VST> void travPre(VST& visit) { //先序遍历
        if (_root) _root->travPre(visit);
    }
    template <typename VST> void travIn(VST& visit) { //中序遍历
        if (_root) _root->travIn(visit);
    }
    template <typename VST> void travPost(VST& visit) { //后序遍历
        if (_root) _root->travPost(visit);
    }

    bool operator<(BinTree<T> const& t) { return _root && t._root && lt(_root, t._root); } //比较器
    bool operator==(BinTree<T> const& t) { return _root && t._root && (_root == t._root); } //判等器
};

template <typename T> int BinTree<T>::updateHeight(BinNodePosi(T) x) { //更新节点x高度
    return x->height = 1 + __max(stature(x->lc), stature(x->rc));
}

template <typename T> void BinTree<T>::updateHeightAbove(BinNodePosi(T) x) { //更新高度
    while (x) {
        updateHeight(x);
        x = x->parent;
    }
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRoot(T const& e) { //将e当作根节点插入空的二叉树
    _size        = 1;
    return _root = new BinNode<T>(e);
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsLC(BinNodePosi(T) x, T const& e) { // e插入为x的左孩子
    _size++;
    x->insertAsLC(e);
    updateHeightAbove(x);
    return x->lc;
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRC(BinNodePosi(T) x, T const& e) { // e插入为x的右孩子
    _size++;
    x->insertAsRC(e);
    updateHeightAbove(x);
    return x->rc;
}

//二叉树子树接入算法: 将S当作节点x的左子树接入, S本身置空
template <typename T> BinNodePosi(T) BinTree<T>::attachAsLC(BinNodePosi(T) x, BinTree<T>*& S) {
    if (S->_root) {               //若左子树非空, (nullptr隐式转换为false)
        x->lc         = S->_root; //插入S根节点为x的左孩子
        x->lc->parent = x;        //更新S根节点的父节点为x
    }
    _size += S->_size;    //更新全树规模
    updateHeightAbove(x); //更新x所有祖先的高度
    S->_root = nullptr;   //释放原树
    S->_size = 0;
    if (S) delete S;
    S = nullptr;
    return x; //返回接入位置
}

//二叉树子树接入算法: 将S当作节点x的右子树接入, S本身置空
template <typename T> BinNodePosi(T) BinTree<T>::attachAsRC(BinNodePosi(T) x, BinTree<T>*& S) {
    if (S->_root) {               //若右子树非空, (nullptrÏ隐式转换为false)
        x->rc         = S->_root; //插入S根节点为x的右孩子
        x->rc->parent = x;        //更新S根节点的父节点为x
    }
    _size += S->_size;    //更新全树规模
    updateHeightAbove(x); //更新x所有祖先的高度
    S->_root = nullptr;   //释放原树
    S->_size = 0;
    if (S) delete S;
    S = nullptr;
    return x; //返回接入位置
}

#endif