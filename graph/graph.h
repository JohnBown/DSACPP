#include "../queue/queueList.h"
#include "../stack/stackVector.h"

#ifndef GRAPH_H
#define GRAPH_H

typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus;          //顶点状态
typedef enum { UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD } EType; //边在遍历树中所属点类型

template <typename Tv, typename Te> class Graph {
  private:
    void reset() {
        for (int i = 0; i < n; i++) {
            status(i) = UNDISCOVERED;
            dTime(i) = fTime(i) = -1;
            parent(i)           = -1;
            priority(i)         = __INT_MAX__;
            for (int j = 0; j < n; j++) {
                if (exists(i, j)) type(i, j) = UNDETERMINED;
            }
        }
    }
    void BFS(int, int&);                      //(连通域)广度优先搜索算法
    void DFS(int, int&);                      //(连通域)深度优先搜索算法
    void BCC(int, int&, Stack<int>&);         //(连通域)基于DFS的双连通分量分解算法
    bool TSort(int, int&, Stack<Tv>*);        //(连通域)基于DFS的拓扑排序算法
    template <typename PU> void PFS(int, PU); //(连通域)优先级搜索框架

  public:
    //顶点
    int n;                             //顶点总数
    virtual int insert(Tv const&) = 0; //插入顶点, 返回编号
    virtual Tv remove(int)        = 0; //删除顶点及其关联边, 返回该顶点信息
    virtual Tv& vertex(int)       = 0; //顶点v的数据(该顶点的确存在)
    virtual int inDegree(int)     = 0; //顶点v的入度(该顶点的确存在)
    virtual int outDegree(int)    = 0; //顶点v的出度(该顶点的确存在)
    virtual int firstNbr(int)     = 0; //顶点v的首个邻接顶点
    virtual int nextNbr(int, int) = 0; //顶点v的(相对于顶点j的)下一个邻接顶点
    virtual VStatus& status(int)  = 0; //顶点v的状态
    virtual int& dTime(int)       = 0; //顶点v的时间标签dTime
    virtual int& fTime(int)       = 0; //顶底v的时间标签fTime
    virtual int& parent(int)      = 0; //顶点v在遍历树中的父亲
    virtual int& priority(int)    = 0; //顶点v在遍历树中的优先级数
    //边: 这里约定, 无向边均统一转化为方向互逆的一对有向边, 从而将无向图视作有向图的特例
    int e;                                             //边总数
    virtual bool exists(int, int)                 = 0; //边(v, u)是否存在
    virtual void insert(Te const&, int, int, int) = 0; //在顶点v和u之间插入权重为w的边e
    virtual Te remove(int, int)                   = 0; //删除顶点v和u之间的边e, 返回该信息
    virtual EType& type(int, int)                 = 0; //边(v, u)的类型
    virtual Te& edge(int, int)                    = 0; //边(v, u)的数据(边的确存在)
    virtual int& weight(int, int)                 = 0; //边(v, u)权重
    //算法
    void bfs(int);                            //广度优先搜索算法
    void dfs(int);                            //深度优先搜索算法
    void bcc(int);                            //基于DFS的双连通分量分解算法
    Stack<Tv>* tSort(int);                    //基于DFS的拓扑排序算法
    void prim(int);                           //最小支撑树Prim算法
    void dijkstra(int);                       //最短路径Dijkstra算法
    template <typename PU> void pfs(int, PU); //优先级搜索树
};

template <typename Tv, typename Te>
void Graph<Tv, Te>::BFS(int v, int& clock) { //广度优先搜索算法(单个连通域)
    Queue<int> Q;                            //辅助队列
    status(v) = DISCOVERED;                  //初始化起点
    Q.enqueue(v);                            //起点入队
    while (!Q.empty()) {
        int v    = Q.dequeue(); //取出队首
        dTime(v) = ++clock;
        for (int u = firstNbr(v); - 1 < u; u = nextNbr(v, u)) { //枚举v的所有邻居u
            if (UNDISCOVERED == status(u)) {                    //若u尚未被发现, 则
                status(u) = DISCOVERED;                         //发现该顶点
                Q.enqueue(u);                                   //该点入队
                type(v, u) = TREE;                              //引入树边, 拓展支撑树
                parent(u)  = v;                                 //该节点的父亲节点为v
            } else {                //若u已被发现, 或者甚至已访问完毕, 则
                type(v, u) = CROSS; //将(v, u)归类于跨边
            }
        }
        status(v) = VISITED; //至此, 当前顶点访问完毕
    }
}

template <typename Tv, typename Te> void Graph<Tv, Te>::bfs(int s) { //广度优先搜索算法(全图)
    reset();                                                         //初始化
    int clock = 0;
    int v     = s;
    do {
        if (UNDISCOVERED == status(v)) BFS(v, clock); //一旦遇到尚未发现的顶点, 从该点出发BFS
    } while (s != (v = (++v % n)));                   //按序号检查, 故不重不漏
}

#endif