#include "quadlistNode.h" //引入Quadlist节点类

#ifndef QUADLIST_H
#define QUADLIST_H

template <typename T> class Quadlist { // Quadlist模版类
  private:
    int _size;                //规模
    QlistNodePosi(T) header;  //头哨兵
    QlistNodePosi(T) trailer; //尾哨兵

  protected:
    void init(); // Quadlist创建时的初始化
    int clear(); //清除所有节点

  public:
    //构造函数
    Quadlist() { init(); } //默认
    //析构函数
    ~Quadlist() { //删除所有节点, 释放哨兵
        clear();
        delete header;
        delete trailer;
    }
    //只读访问接口
    Rank size() const { return _size; }                                             //规模
    bool empty() const { return _size <= 0; }                                       //判空
    QlistNodePosi(T) first() const { return header->succ; }                         //首节点位置
    QlistNodePosi(T) last() const { return trailer->pred; }                         //末节点位置
    bool valid(QlistNodePosi(T) p) { return p && (trailer != p) && (header != p); } //判断位置p是否对外合法
    //可写访问接口
    T remove(QlistNodePosi(T) p); //删除(合法)位置p处的节点, 返回被删除节点的数值
    QlistNodePosi(T)              //将*e作为p的后继, b的上邻插入
        insertAfterAbove(T const& e, QlistNodePosi(T) p, QlistNodePosi(T) b = nullptr);
    //遍历
    void traverse(void (*)(T&)); //遍历各节点, 依次实施指定操作(函数指针, 只读或局部修改)
    template <typename VST> void traverse(VST&); //遍历各节点, 依次实施指定操作(函数对象, 可全局性修改节点)
};

template <typename T> void Quadlist<T>::init() { // Quadlist初始化, 创建Quadlist对象时统一调用
    header        = new QuadlistNode<T>;         //创建头哨兵节点
    trailer       = new QuadlistNode<T>;         //创建尾哨兵节点
    header->succ  = trailer;
    header->pred  = nullptr; //沿横向联接头哨兵
    trailer->pred = header;
    trailer->succ = nullptr;                  //沿横向联接尾哨兵
    header->above = trailer->above = nullptr; //纵向的后继置空
    header->below = trailer->below = nullptr; //纵向的前继置空
    _size                          = 0;       //记录规模
} //如此构造的四联表, 不含任何实质的节点, 且暂时与其他四联表相互独立

template <typename T>
T Quadlist<T>::remove(QlistNodePosi(T) p) { //删除Quadlist内位置p处的节点, 返回其中存放的词条
    p->perd->succ = p->succ;
    p->succ->pred = p->pred; //摘除节点
    _size--;
    T e = p->entry;
    delete p; //备份词条, 释放节点
    return e; //返回词条
}

template <typename T> int Quadlist<T>::clear() { //清空Quadlist
    int oldSize = _size;
    while (0 < _size) remove(header->succ); //逐个删除所有节点
    return oldSize;
}

template <typename T> //将e作为p的后继, b的上邻插入Quadlist
QlistNodePosi(T) Quadlist<T>::insertAfterAbove(T const& e, QlistNodePosi(T) p, QlistNodePosi(T) b) {
    _size++;
    return p->insertAsSuccAbove(e, b); //返回新节点位置(below = nullptr), b = nullptr
}

template <typename T> void Quadlist<T>::traverse(void (*visit)(T&)) { //利用函数指针机制, 只读或局部性修改
    QlistNodePosi(T) p = header;
    while ((p = p->succ) != trailer) visit(p->data);
}

template <typename T>
template <typename VST>
void Quadlist<T>::traverse(VST& visit) { //利用函数对象机制, 可全局性修改
    QlistNodePosi(T) p = header;
    while ((p = p->succ) != trailer) visit(p->data);
}

#endif