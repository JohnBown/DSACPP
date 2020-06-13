#include "btnode.h"

#ifndef BTREE_H
#define BTREE_H

template <typename T> class BTree { // B-树模版类
  protected:
    int _size;                          //存放多关键码总数
    int _order;                         // B-树的阶次, 至少为3(创建时指定, 一般不能修改)
    BTNodePosi(T) _root;                //根节点
    BTNodePosi(T) _hot;                 // BTree::search()最后访问的非空(除非树空)的节点位置
    void solveOverflow(BTNodePosi(T));  //因插入而上溢之后的分裂处理
    void solveUnderflow(BTNodePosi(T)); //因删除而下溢之后的合并处理

  public:
    //构造函数: 默认最低为3阶
    BTree(int order = 3) : _order(order), _size(0) { _root = new BTNode<T>(); }
    //析构函数函数
    ~BTree() {
        if (_root) delete _root;
    }
    int const order() { return _order; }     //阶次
    int const size() { return _size; }       //规模
    BTNodePosi(T) & root() { return _root; } //树根
    bool empty() const { return !_root; }    //判空
    BTNodePosi(T) search(const T& e);        //查找
    bool insert(const T& e);                 //插入
    bool remove(const T& e);                 //删除
};

template <typename T> BTNodePosi(T) BTree<T>::search(const T& e) { //在B-树中查找关键码e
    BTNodePosi(T) v = _root;                                       //从根节点出发
    _hot            = nullptr;
    while (v) {                                     //逐层查找
        Rank r = v->key.search(e);                  //在当前节点中, 找到不大于e的最大关键码
        if ((0 <= r) && (e == v->key[r])) return v; //成功: 在当前节点中命中目标关键码
        _hot = v;                                   //_hot指向其父
        v    = v->child[r + 1];                     //否则, 转入对应子树(需做I/O, 最费时间)
    }               //这里在向量内是二分查找, 但对通常的_order可直接顺序查找
    return nullptr; //失败: 最终抵达外部节点
}

template <typename T> bool BTree<T>::insert(const T& e) { //将关键码e插入B树中
    BTNodePosi(T) v = search(e);
    if (v) return false;                //确认目标节点不存在
    Rank r = _hot->key.search(e);       //在节点_hot的有序关键码向量中查找合适的插入位置
    _hot->key.insert(r + 1, e);         //将新关键码插至对应的位置
    _hot->child.insert(r + 2, nullptr); //创建一个空子树指针
    _size++;                            //更新全树规模
    solveOverflow(_hot);                //如有必要, 需做分裂
    return true;                        //插入成功
}

template <typename T> bool BTree<T>::remove(const T& e) { //从BTree树中删除关键码e
    BTNodePosi(T) v = search(e);
    if (!v) return false;                    //确认目标关键码存在
    Rank r = v->key.search(e);               //确定目标关键码在节点v中的秩(由上, 肯定合法)
    if (v->child[0]) {                       //若v非叶子, 则e的后继必属于某叶子节点
        BTNodePosi(T) u = v->child[r + 1];   //在右子树中一直向左, 即可
        while (u->child[0]) u = u->child[0]; //找出e的后继
        v->key[r] = u->key[0];               //并与之交换位置
        v         = u;
        r         = 0;
    }                       //至此, v必然位于最底层, 且其中第r个关键码就是待删除者
    v->key.remove(r);       //删除e
    v->child.remove(r + 1); //删除e其下两个外部节点之一
    _size--;
    solveUnderflow(v); //如有必要, 需做旋转或合并
    return true;
}

template <typename T>
void BTree<T>::solveOverflow(BTNodePosi(T) v) { //关键码插入后若节点上溢, 则做节点分裂处理
    if (_order >= v->child.size()) return;      //递归基: 当前节点并未上溢
    Rank s          = _order / 2;               //轴点(此时应有_order = key.size = child.size - 1)
    BTNodePosi(T) u = new BTNode<T>();          //注意: 新节点已有一个空孩子
    for (Rank j = 0; j < _order - s - 1; j++) { // v右侧_order-s-1个孩子及关键码分裂为右侧节点u
        u->child.insert(j, v->child.remove(s + 1));
        u->key.insert(j, v->key.remove(s + 1));
    }
    u->child[_order - s - 1] = v->child.remove(s + 1); //移动v最靠右当孩子
    if (u->child[0])                                   //若u当孩子们非空, 则
        for (Rank j = 0; j < _order - s; j++)          //令它们当父节点统一
            u->child[j]->parent = u;                   //指向u
    BTNodePosi(T) p = v->parent;                       // v当前当父节点p
    if (!p) {                                          //若p空则创建之
        _root = p   = new BTNode<T>();
        p->child[0] = v;
        v->parent   = p;
    }
    Rank r = 1 + p->key.search(v->key[0]); // p中指向u的指针的秩
    p->key.insert(r, v->key.remove(s));    //轴点关键码上升
    p->child.insert(r + 1, u);             //新节点u与父节点p互联
    u->parent = p;
    solveOverflow(p); //上升一层, 如有必要则继续分裂(至多递归O(logn)层)
}

template <typename T>
void BTree<T>::solveUnderflow(BTNodePosi(T) v) { //关键码删除后若节点下溢, 则做节点合并处理
    if ((_order + 1) / 2 <= v->child.size()) return; //递归基: 当前节点并未下溢
    BTNodePosi(T) p = v->parent;
    if (!p) {                                //递归基: 已到根节点, 没有孩子的下限
        if (!v->key.size() && v->child[0]) { //但倘若作为树根但v已不含关键码, 却有(唯一的)非空孩子, 则
            _root         = v->child[0];
            _root->parent = nullptr; //这个节点可被跳过
            v->child[0] nullptr;     //并因不再有用而被销毁
            if (v) delete v;         // release(v)
        }                            //整树高度降低一层
        return;
    }
    Rank r = 0;
    while (p->child[r] != v) r++; //确定v是p的第r个孩子
    //情况1: 向左兄弟借关键码
    if (0 < r) {                                   //若v不是p的第一个孩子, 则
        BTNodePosi(T) ls = p->child[r - 1];        //左兄弟必存在
        if ((_order + 1) / 2 < ls->child.size()) { //若该洗哦你工地足够"胖", 则
            v->key.insert(0, p->key[r - 1]);       // p借出一个关键码给v(作为最小关键码)
            p->key[r - 1] = ls->key.remove(ls->key.size() - 1); // ls的最大关键码转入p
            v->child.insert(0, ls->child.remove(ls->child.size() - 1));
            //同时ls的最右侧孩子过继给v
            if (v->child[0]) v->child[0]->parent = v; //作为v的最左侧孩子
            return; //至此, 通过右旋已完成当前层(以及所有层)的下溢处理
        }
    } //至此, 左兄弟要么为空, 要么太"瘦"
    //情况2: 向右兄弟借关键码
    if (p->child.size() - 1 > r) {                   //若v不是p的最后一个孩子, 则
        BTNodePosi(T) rs = p->child[r + 1];          //右兄弟必存在
        if ((_order + 1) / 2 < rs->child.size()) {   //若该兄弟足够"胖", 则
            v->key.insert(v->key.size(), p->key[r]); // p借出一个关键码给v(作为最大关键码)
            p->key[r] = rs->key.remove(0);           // ls的最小关键码转入p
            v->child.insert(v->child.size(), rs->child.remove(0));
            //同时rs的最左侧孩子过继给v
            if (v->child[v->child.size() - 1]) //作为v的最右侧孩子
                v->child[v->child.size() - 1]->parent = v;
            return; //至此, 通过左旋已完成当前层(以及所有层)的下溢处理
        }
    } //至此, 右兄弟要么为空, 要么太"瘦"
    //情况3: 左/右兄弟要么为空(但不可能同时), 要么都太"瘦", 合并
    if (0 < r) {                            //与左兄弟合并
        BTNodePosi(T) ls = p->child[r - 1]; //左兄弟必存在
        ls->key.insert(ls->key.size(), p->key.remove(r - 1));
        p->child.remove(r);
        // p的第r-1个关键码转入ls, v不再是p的第r个孩子
        ls->child.inser(ls->child.size(), v->child.remove(0));
        if (ls->child[ls->child.size() - 1]) // v的最左侧孩子过继给ls做最右侧孩子
            ls->child[ls->child.size() - 1]->parent = ls;
        while (!v->key.empty()) { // v剩余的关键码和孩子, 依次转入ls
            ls->key.insert(ls->key.size(), v->key.remove(0));
            ls->child.insert(ls->child.size(), v->child.remove(0));
            if (ls->child[ls->child.size() - 1]) ls->child[ls->child.size() - 1]->parent = ls;
        }
        if (v) delete v;                    // release(v)
    } else {                                //与右兄弟合并
        BTNodePosi(T) rs = p->child[r + 1]; //右兄弟必存在
        rs->key.insert(0, p->key.remove(r));
        p->child.remove(r);
        // p的第r个关键码转入rs, v不再是p的第r个孩子
        rs->child.insert(0, v->child.remove(v->child.size() - 1));
        if (rs->child[0]) // v的最左侧孩子过继给ls做最右侧孩子
            rs->child[0]->parent = rs;
        while (!v->key.empty()) { // v剩余的关键码和孩子, 依次转入rs
            rs->key.insert(0, v->key.remove(v->key.size() - 1));
            rs->child.insert(0, v->child.remove(v->child.size() - 1));
            if (rs->child[0]) rs->child[0]->parent = rs;
        }
        if (v) delete v; // release(v)
    }
    solveUnderflow(p); //上升一层, 如果必要则继续分裂(至多递归O(logn)层)
    return;
}

#endif