#include "../queue/queueList.h"
#include "../stack/stackVector.h"

#ifndef BIN_NODE_H
#define BIN_NODE_H

#define __max(a, b) (((a) > (b)) ? (a) : (b))
#define __min(a, b) (((a) < (b)) ? (a) : (b))

// BASIC
#define IsRoot(x) (!((x).parent))
#define IsLChild(x) (!IsRoot(x) && (&(x) == (x).parent->lc))
#define IsRChild(x) (!IsRoot(x) && (&(x) == (x).parent->rc))
#define HasParent(x) (!IsRoot(x))
#define HasLChild(x) ((x).lc)
#define HasRChild(x) ((x).rc)
#define HasChild(x) (HasLChild(x) || HasRChild(x))
#define HasBothChild(x) (HasLChild(x) && HasRChild(x))
#define IsLeaf(x) (!HasChild(x))

#define sibling(p) (IsLChild(*(p)) ? (p)->parent->rc : (p)->parent->lc)                         //兄弟
#define uncle(x) (IsLChild(*((x)->parent)) ? (x)->parent->parent->rc : (x)->parent->parent->lc) //叔叔
#define FromParentTo(x) (IsRoot(x) ? _root : (IsLChild(x) ? (x).parent->lc : (x).parent->rc)) //来自父亲的引用

// AVL
#define Balanced(x) (stature((x).lc) == stature((x).rc))
#define BalFac(x) (stature((x).lc) - stature((x).rc))
#define AvlBalanced(x) ((-2 < BalFac(x)) && (BalFac(x) < 2))

#define HeightUpdated(x) ((x).height == 1 + __max(stature((x).lc), stature((x).rc)))

// RedBlack
// TODO

#define BinNodePosi(T) BinNode<T>*         //节点位置
#define stature(p) ((p) ? p->height : -1)  //节点高度(空树高度为-1)
typedef enum { RB_RED, RB_BLACK } RBColor; //节点颜色

//二叉树节点模版类
template <typename T> struct BinNode {
    //成员
    T data;                //数值
    BinNodePosi(T) parent; //父节点
    BinNodePosi(T) lc;     //左孩子节点
    BinNodePosi(T) rc;     //右孩子节点
    int height;            //高度(通用)
    int npl;               // Null Path Length(左式堆)
    RBColor color;         //颜色(红黑树)
    //构造函数
    BinNode() : parent(nullptr), lc(nullptr), rc(nullptr), height(0), npl(1), color(RB_RED) {}
    BinNode(T e, BinNodePosi(T) p = nullptr, BinNodePosi(T) lc = nullptr, BinNodePosi(T) rc = nullptr,
            int h = 0, int l = 1, RBColor c = RB_RED) :
        data(e), parent(p), lc(lc), rc(rc), height(h), npl(l), color(c) {}
    //操作接口
    int size(); //统计当前节点后代总数, 亦即以其为根的子树的规模
    BinNodePosi(T) insertAsLC(T const&);          //作为当前节点的左孩子插入新节点
    BinNodePosi(T) insertAsRC(T const&);          //作为当前节点的右孩子插入新节点
    BinNodePosi(T) succ();                        //取当前节点的直接后继
    template <typename VST> void travLevel(VST&); //子树层次遍历
    template <typename VST> void travPre(VST&);   //子树先序遍历
    template <typename VST> void travIn(VST&);    //子树中序遍历
    template <typename VST> void travPost(VST&);  //子树后序遍历
    //比较器, 判等器
    bool operator<(BinNode const& bn) { return data < bn.data; }   //小于
    bool operator==(BinNode const& bn) { return data == bn.data; } //等于
    // DSA
    BinNodePosi(T) zag(); //逆时针旋转
    BinNodePosi(T) zig(); //顺时针旋转
};

template <typename T> int BinNode<T>::size() { //统计当前节点后代总数, 即为其根的子树规模
    int s = 1;                                 //计入本身
    if (lc) s += lc->size();                   //递归计入左子树规模
    if (rc) s += rc->size();                   //递归计入右子树规模
    return s;
}

template <typename T> BinNodePosi(T) BinNode<T>::insertAsLC(T const& e) { //将e作为当前节点的左孩子插入二叉树
    return lc = new BinNode(e, this);
}

template <typename T> BinNodePosi(T) BinNode<T>::insertAsRC(T const& e) { //将e作为当前节点的右孩子插入二叉树
    return rc = new BinNode(e, this);
}

template <typename T> BinNodePosi(T) BinNode<T>::succ() { //定位节点v的直接后继
    BinNodePosi(T) s = this;                              //记录后继的临时变量
    if (rc) {                            //若有右孩子, 则直接后继必在右子树中, 具体地就是
        s = rc;                          //右子树中
        while (HasLChild(*s)) s = s->lc; //最靠近(最小)的节点
    } else { //否则, 直接后继应是"将当前节点包含于其左子树中的最低祖先"
        while (IsRChild(*s)) s = s->parent; //逆向地沿右向分支, 不断朝左上方移动
        s = s->parent; //然后再朝右上方移动一步, 即抵达直接后继(如果存在)
    }
    return s;
}

/*** 先序遍历 ***/

template <typename T, typename VST>
void travPre_R(BinNodePosi(T) x, VST& visit) { //二叉树先序遍历算法(递归版)
    if (!x) return;
    visit(x->data);
    travPre_R(x->lc, visit);
    travPre_R(x->rc, visit);
}

template <typename T, typename VST>
void travPre_I1(BinNodePosi(T) x, VST& visit) { //二叉树先序遍历算法(迭代版1)
    Stack<BinNodePosi(T)> S;                    //辅助栈
    if (x) S.push(x);                           //根节点入栈
    while (!S.empty()) {                        //在栈变空之前反复循环
        x = S.pop();                            //弹出并访问当前节点
        visit(x->data);
        if (HasRChild(*x)) S.push(x->rc); //非空孩子的入栈次序为先右后左
        if (HasLChild(*x)) S.push(x->lc);
    }
}

template <typename T, typename VST>
static void visitAlongLeftBranch(BinNodePosi(T) x, VST& visit, Stack<BinNodePosi(T)>& S) {
    // 从当前节点出发, 沿左分支不断深入, 直至没有左分支的节点; 沿途节点遇到后立即访问
    while (x) {
        visit(x->data); //访问当前节点
        S.push(x->rc);  //右孩子入栈暂存
        x = x->lc;      //沿左分支深入一层
    }
}

template <typename T, typename VST>
void travPre_I2(BinNodePosi(T) x, VST& visit) { //二叉树先序遍历算法(迭代版2)
    Stack<BinNodePosi(T)> S;                    //辅助栈
    while (true) {
        visitAlongLeftBranch(x, visit, S); //从当前节点出发, 逐批访问
        if (S.empty()) break;              //直至栈空
        x = S.pop();                       //弹出下一批的起点
    }
}

template <typename T> template <typename VST> void BinNode<T>::travPre(VST& visit) { //二叉树先序遍历
    // return travPre_R(this, visit);
    // return travPre_I1(this, visit);
    return travPre_I2(this, visit);
}

/*** 中序遍历 ***/

template <typename T, typename VST> void travIn_R(BinNodePosi(T) x, VST& visit) { //二叉树中序遍历算法(递归版)
    if (!x) return;
    travIn_R(x->lc, visit);
    visit(x->data);
    travIn_R(x->rc, visit);
}

template <typename T> static void goAlongLeftBranch(BinNodePosi(T) x, Stack<BinNodePosi(T)>& S) {
    //从当前节点出发, 沿左分支不断深入, 直至没有左分支的节点
    while (x) {
        S.push(x); //当前节点入栈
        x = x->lc; //随即向左侧分支深入, 迭代至无左孩子
    }
}

template <typename T, typename VST>
void travIn_I1(BinNodePosi(T) x, VST& visit) { //二叉树中序遍历算法(迭代版1)
    Stack<BinNodePosi(T)> S;                   //辅助栈
    while (true) {
        goAlongLeftBranch(x, S); //从当前节点出发, 逐批入栈
        if (S.empty()) break;    //直至所有节点处理完毕
        x = S.pop();             //弹出栈顶节点并访问之
        visit(x->data);
        x = x->rc; //转向右子树
    }
}

template <typename T, typename VST>
void travIn_I2(BinNodePosi(T) x, VST& visit) { //二叉树中序遍历算法(迭代版2)
    Stack<BinNodePosi(T)> S;                   //辅助栈
    while (true) {
        if (x) {
            S.push(x); //根节点入栈
            x = x->lc; //深入遍历左子树
        } else if (!S.empty()) {
            x = S.pop;      //尚未访问的最低祖先节点出栈
            visit(x->data); //访问该祖先节点
            x = x->rc;      //遍历祖先的右子树
        } else {
            break; //遍历完成
        }
    }
}

template <typename T, typename VST>
void travIn_I3(BinNodePosi(T) x, VST& visit) { //二叉树中序遍历算法(迭代版3)
    bool backtrack = false;                    //前一步是否刚从右子树回溯
    while (true) {
        if (!backtrack && HasLChild(*x)) { //若不是刚刚回溯, 且含有左子树
            x = x->lc;                     //深入遍历左子树
        } else {                           //否则: 无左子树或刚刚回溯
            visit(x->data);                //访问该节点
            if (HasRChild(*x)) {           //若其右子树非空, 则
                x         = x->rc;         //深入右子树继续遍历
                backtrack = false;         //并关闭回溯标志
            } else {
                if (!(x = x->succ())) break; //回溯(抵达末节点推出)
                backtrack = true;            //并设置回溯位
            }
        }
    }
}

template <typename T, typename VST>
void travIn_I4(BinNodePosi(T) x, VST& visit) { //二叉树中序遍历算法(迭代版4)
    while (true) {
        if (HasLChild(*x)) {            //如果有左子树, 则
            x = x->lc;                  //深入遍历左子树
        } else {                        //否则
            visit(x->data);             //访问当前节点, 并
            while (!HasRChild(*x)) {    //不断的在无右分支处
                if (!(x = x->succ())) { //回溯至直接后继
                    return;             //末节点处直接推出
                } else {
                    visit(x->data); //访问新的当前节点
                }
            }
            x = x->rc; //(直至有右分支处)转向非空的右子树
        }
    }
}

template <typename T> template <typename VST> void BinNode<T>::travIn(VST& visit) { //二叉树中序遍历
    // return travIn_R(this, visit);
    // return travIn_I1(this, visit);
    // return travIn_I2(this, visit);
    // return travIn_I3(this, visit);
    return travIn_I4(this, visit);
}

/*** 后序遍历 ***/

template <typename T, typename VST> void travPost_R(BinNodePosi(T) x, VST& visit) { //二叉树后序遍历(递归版)
    if (!x) return;
    travPost_R(x->lc, visit);
    travPost_R(x->rc, visit);
    visit(x->data);
}

template <typename T>
static void gotoHLVFL(Stack<BinNodePosi(T)>& S) { //以S栈顶节点为根的子树中, 找到最高左侧可见叶节点
    while (BinNodePosi(T) x = S.top()) {      //自顶向下, 反复检查当前节点(即栈顶)
        if (HasLChild(*x)) {                  //尽可能向左
            if (HasRChild(*x)) S.push(x->rc); //若有右孩子, 优先入栈
            S.push(x->lc);                    //然后才转至左孩子
        } else {                              //实不得已
            S.push(x->rc);                    //才向右
        }
    }
    S.pop(); //返回之前, 弹出栈顶的空节点
}

template <typename T, typename VST> void travPost_I(BinNodePosi(T) x, VST& visit) { //二叉树后序遍历(迭代版)
    Stack<BinNodePosi(T)> S;                                                        //辅助栈
    if (x) S.push(x);                                                               //根节点入栈
    while (!S.empty()) {
        if (S.top() != x->parent) { //若栈顶非当前节点之父(则必为其右兄), 此时需
            gotoHLVFL(S);           //在以其右兄为根之子树中, 找到HLVFL
        }
        x = S.pop();    //弹出栈顶(即前一节点之后继)
        visit(x->data); //并访问之
    }
}

template <typename T> template <typename VST> void BinNode<T>::travPost(VST& visit) { //二叉树后序遍历
    // return travPost_R(this, visit);
    return travPost_I(this, visit);
}

/*** 层次遍历 ***/

template <typename T> template <typename VST> void BinNode<T>::travLevel(VST& visit) { //二叉树层次遍历
    Queue<BinNodePosi(T)> Q;                                                           //辅助队列
    Q.enqueue(this);                                                                   //根节点入队
    while (!Q.empty()) {                     //队列非空, 反复迭代
        BinNodePosi(T) x = Q.dequeue();      //取出队首节点
        visit(x->data);                      //访问之
        if (HasLChild(*x)) Q.enqueue(x->lc); //左孩子入队
        if (HasRChild(*x)) Q.enqueue(x->rc); //右孩子入队
    }
}

template <typename T> BinNodePosi(T) BinNode<T>::zag() { //逆时针旋转
    BinNodePosi(T) rChild = rc;
    rChild->parent        = this->parent;
    if (rChild->parent) ((this == rChild->parent->lc) ? rChild->parent->lc : rChild->parent->rc) = rChild;
    rc = rChild->lc;
    if (rc) rc->parent = this;
    rChild->lc   = this;
    this->parent = rChild;
    // update heights
    height         = 1 + __max(stature(lc), stature(rc));
    rChild->height = 1 + __max(stature(rChild->lc), stature(rChild->rc));
    for (BinNodePosi(T) x = rChild->parent; x; x->parent) {
        if (HeightUpdated(*x))
            break;
        else
            x->height = 1 + __max(stature(x->lc), stature(x->rc));
    }
    return rChild;
}

template <typename T> BinNodePosi(T) BinNode<T>::zig() { //顺时针旋转
    BinNodePosi(T) lChild = lc;
    lChild->parent        = this->parent;
    if (lChild->parent) ((this == lChild->parent->rc) ? lChild->parent->rc : lChild->parent->lc) = lChild;
    lc = lChild->rc;
    if (lc) lc->parent = this;
    lChild->rc   = this;
    this->parent = lChild;
    // update height
    height         = 1 + __max(stature(lc), stature(rc));
    lChild->height = 1 + __max(stature(lChild->lc), stature(lChild->rc));
    for (BinNodePosi(T) x = lChild->parent; x; x->parent) {
        if (HeightUpdated(*x))
            break;
        else
            x->height = 1 + __max(stature(x->lc), stature(x->rc));
    }
    return lChild;
}

#endif