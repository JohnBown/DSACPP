#include <stdlib.h>
#include <time.h>

#include "queueList.h"

struct Customer {      //顾客类
    int window;        //所属窗口(队列)
    unsigned int time; //服务时长
};

int bestWindow(Queue<Customer> windows[], int nWin) { //为新到顾客确定最佳队列
    int minSize = windows[0].size(), optiWin = 0;
    for (int i = 1; i < nWin; i++) {
        if (minSize > windows[i].size()) {
            minSize = windows[i].size();
            optiWin = i;
        }
    }
    return optiWin;
}

void simulate(int nWin, int servTime) { //按指定窗口数, 服务总时间模拟银行业务
    Queue<Customer> *windows = new Queue<Customer>[nWin]; //每个窗口创建一个队列
    for (int now = 0; now < servTime; now++) {            //在服务时间消耗结束之前
        if (rand() % (1 + nWin)) {                        //新顾客以nWin / (1 + nWin)概率到达
            Customer c;
            c.time   = 1 + rand() % 98;
            c.window = bestWindow(windows, nWin); //插入最佳(最短)服务窗口
            windows[c.window].enqueue(c);
        }
    }
    for (int i = 0; i < nWin; i++) { //分别检查
        if (!windows[i].empty()) {
            if (--windows[i].front().time < 0) { //队首顾客服务时间减一单位
                windows[i].dequeue();            //服务完毕，出队列
            }
        }
    }
    delete[] windows;
}

int main() {}