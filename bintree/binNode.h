#ifndef BIN_NODE
#define BIN_NODE

#define __max(a, b) (((a) > (b)) ? (a) : (b))
#define __min(a, b) (((a) < (b)) ? (a) : (b))

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
    BinNode(T e, BinNodePosi(T) p = nullptr, BinNodePosi(T) lc = nullptr,
            BinNodePosi(T) rc = nullptr, int h = 0, int l = 1, RBColor c = RB_RED) :
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
};

template <typename T> int BinNode<T>::size() { //统计当前节点后代总数, 即为其根的子树规模
    int s = 1;                                 //计入本身
    if (lc) s += lc->size();                   //递归计入左子树规模
    if (rc) s += rc->size();                   //递归计入右子树规模
    return s;
}

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsLC(T const& e) { //将e作为当前节点的左孩子插入二叉树
    return lc = new BinNode(e, this);
}

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsRC(T const& e) { //将e作为当前节点的右孩子插入二叉树
    return rc = new BinNode(e, this);
}

#endif