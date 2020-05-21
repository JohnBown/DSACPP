#ifndef QUEUE_LIST_H
#define QUEUE_LIST_H

#include "../list/list.h" //以列表为基, 派生出队列模版类

template <typename T> class Queue : public List<T> { //队列模版类
  public:
    void enqueue(T const &e) { this->insertAsLast(e); } //入队: 尾部插入
    T dequeue() { return this->remove(this->fist()); }  //出队: 首部删除
    T &front() { return this->first()->data; }          //队首
};

#endif