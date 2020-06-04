#include "../vector/vector.h"
#include "graph.h"

#ifndef GRAPH_MATRIX_H
#define GRAPH_MATRIX_H

template <typename Tv> struct Vertex { //顶点对象
    Tv data;                           //数据
    int inDegree, outDegree;           //入度, 出度
    VStatus status;                    //状态
    int dTime, fTime;                  //时间标签
    int parent;                        //在你遍历树中的父节点
    int priority;                      //在遍历树中的优先级数
    Vertex(Tv const& d = (Tv)0) :
        data(d),
        inDegree(0),
        outDegree(0),
        status(UNDISCOVERED),
        dTime(-1),
        fTime(-1),
        parent(-1),
        priority(__INT_MAX__) {} //构造新顶点
};

template <typename Te> struct Edge {                                     //边对象
    Te data;                                                             //数据
    int weight;                                                          //权重
    Etype type;                                                          //类型
    Edge(Te const& d, int w) : data(d), weight(w), type(UNDETERMINED) {} //构造
};

template <typename Tv, typename Te>
class GraphMatrix : public Graph<Tv, Te> { //基于向量, 以邻接矩阵形式实现的图
  private:
    Vector<Vertex<Tv> > V;        //顶点集(向量)
    Vector<Vector<Edge<Te>*> > E; //边集(邻接矩阵)

  public:
    GraphMatrix(n = e = 0;); //构造
    ~GraphMatrix() {         //析构
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) delete E[j][k];
        }
    }
    //顶点基本操作
    virtual Tv& vertex(int i) { return V[i].data; }         //数据
    virtual int inDegree(int i) { return V[i].inDegree; }   //入度
    virtual int outDegree(int i) { return V[i].outDegree; } //出度
    virtual int firstNbr(int i) { return nextNbr(i, n); }   //首个邻接顶点
    virtual int nextNbr(int i, int j) {                     //相对于顶点j的下一个邻接顶点
        while ((-1 < j) && (!exists(i, --j))) {}            //逆向线性试探
        return j;
    }
    virtual VStatus& status(int i) { return V[i].dTime; }  //状态
    virtual int& dTime(int i) { return V[i].dTime; }       //时间标签dTime
    virtual int& fTime(int i) { return V[i].fTime; }       //时间标签fTime
    virtual int& parent(int i) { return V[i].parent; }     //在遍历树中的父亲
    virtual int& priority(int i) { return V[i].priority; } //在遍历树中的优先级树
    //顶点动态操作
    virtual int insert(Tv const& vertex) { //插入顶点, 返回编号
        for (int i = 0; i < n; i++) {
            E[i].insert(nullptr); //各顶点预留一条潜在的关联边
        }
        n++;                                                   //更新计数
        E.insert(Vector<Edge<Te>*>(n, n, (Edge<Te>*)nullptr)); //创建新顶点对应的边向量
        return V.insert(Vertex<Tv>(vertex));                   //顶点向量增加一个顶点
    }
    virtual Tv remove(int i) { //删除第i个顶点及其关联边
        for (int j = 0; j < n; j++) {
            if (exists(i, j)) {  //所有出边
                delete E[i][j];  //逐条删除
                V[j].inDegree--; //更新j入读
            }
        }
        E.remove(i);         //删除第i行
        n--;                 //更新顶点计数
        Tv vBak = vertex(i); //备份
        V.remove(i);         //删除顶点i
        for (int j = 0; j < n; j++) {
            if (Edge<Te>* e = E[j].remove(i)) { //所有入边
                delete e;                       //逐条删除
                V[j].outDegree--;               //更新j出度
            }
        }
        return vBak; //返回被删除信息
    }
    //边的确认操作
    virtual bool exists(int i, int j) { //边(i, j)是否存在
        return (0 <= i) && (i < n) && (0 <= j) && (j < n) && (E[i][j] != NULL);
    }
    //边的基本操作
    virtual EType& type(int i, int j) { return E[i][j]->type; }   //边(i, j)的类型
    virtual Te& edge(int i, int j) { return E[i][j]->data; }      //边(i, j)的数据
    virtual int& weight(int i, int j) { return E[i][j]->weight; } //边(i, j)的权重
    //边的动态操作
    virtual void insert(Te const& edge, int w, int i, int j) { //插入权重为w的边e = (i, j)
        if (exists(i, j)) return;                              //确保该边尚不存在
        E[i][j] = new Edge<Te>(edge, w);                       //创建新边
        e++;                                                   //更新边计数
        V[i].outDegree++;                                      //更新i的出度数
        V[j].inDegree++;                                       //更新j的入度数
    }
    virtual Te remove(int i, int j) { //删除顶点i和j之间的联边
        Te eBak = edge(i, j);         //备份
        delete E[i][j];               //删除
        E[i][j] = nullptr;            //置空
        e--;                          //更新边计数器
        V[i].outDegree--;             //更新i的出度数
        V[j].inDegree--;              //更新j的入度数
        return eBak;                  //返回备份
    }
};

#endif