#ifndef DICTIONARY_H
#define DICTIONARY_H

template <typename K, typename V> struct Dictionary { //词典Dictionary模版类
    virtual int size() const = 0;                     //当前词条总数
    virtual bool put(K, V)   = 0;                     //插入词条(禁止雷同词条)
    virtual V* get(K k)      = 0;                     //读取词条
    virtual bool remove(K k) = 0;                     //删除词条
};

#endif