#ifndef STACK_VECTOR_H
#define STACK_VECTOR_H

#include "../vector/vector.h"

template <typename T> class Stack : public Vector<T> {
  public:
    void push(T const &e) { insert(e); }
    T pop() { return remove(Vector<T>::size() - 1); }
    T &top() { return (*this)[Vector<T>::size() - 1]; }
};

#endif