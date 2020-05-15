#ifndef STACK_LIST_H
#define STACK_LIST_H

#include "../list/list.h" //以列表为基类，派生出栈模版类

template <typename T> class Stack : public List<T> { //将列表的首/末作为栈底/顶
  public:
    void push(T const &e) { this->insertAsLast(e); } //入栈：等效于将新元素作为列表的末元素插入
    T pop() { return this->remove(this->last()); } //出栈：等效于删除一个列表末元素
    T &top() { return this->last()->data; }        //取顶：直接返回列表的末元素
};

#endif