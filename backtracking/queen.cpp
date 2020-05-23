#include "../stack/stackVector.h"
#include <iostream>

using namespace std;

bool IS_SHOW = true; //是否打印棋盘

int nSolu = 0;  //解的总数
int nCheck = 0; //尝试的总数

struct Queen { //皇后类
    int x, y;  //皇后在棋盘上的位置坐标
    Queen(int xx = 0, int yy = 0) : x(xx), y(yy) {}
    bool operator==(Queen const &q) const { //重载判等操作符, 以检测不同皇后间可能发生冲突
        return (x == q.x)                   //行冲突
               || (y == q.y)                //列冲突
               || (x + y == q.x + q.y)      //正对角线冲突
               || (x - y == q.x - q.y);     //反对角线冲突
    }
    bool operator!=(Queen const &q) const { return !(*this == q); } //重载不等操作符
};

void display(int N, Stack<Queen> &solu, Queen &q) { //混乱代码, 可以画棋盘
    char board[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            board[i][j] = ' ';
    board[q.x][q.y] = 'q';
    for (int i = 0; i < solu.size(); i++)
        board[solu[i].x][solu[i].y] = '#';
    printf("|");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%c|", board[i][j]);
        if (i < N - 1)
            printf("\n|");
    }
    printf("\n\n");
}

void placeQueens(int N) { // N皇后算法(迭代版);采用试探/回溯的策略, 借助栈记录查找结果
    Stack<Queen> solu;    //存放(部分)解的栈
    Queen q(0, 0);        //从原点位置出发
    do {                  //反复试探, 回溯
        if (N <= solu.size() || N <= q.y) {            //若已出界, 则
            q = solu.pop();                            //回溯一行
            q.y++;                                     //并继续试探下一列
        } else {                                       //否则, 试探下一行
            while ((q.y < N) && (0 <= solu.find(q))) { //通过与已有皇后比对
                q.y++;                                 //尝试找到可摆放下一皇后的列
                nCheck++;
            }
            if (N > q.y) {              //若存在可摆放的列, 则
                solu.push(q);           //摆放当前皇后, 并
                if (N <= solu.size()) { //若部分解答已成全局解, 则计数
                    nSolu++;
                    if (IS_SHOW)             //如果想打印棋盘, 则
                        display(N, solu, q); //画图显示
                }
                q.x++;   //转入下一行
                q.y = 0; //从第0列开始, 试探下一皇后
            }
        }
    } while ((0 < q.x) || (q.y < N)); //所有分支均已或穷举或剪枝之后, 算法结束
}

int main() {
    placeQueens(4);
    printf("尝试%d次\n", nCheck);
    printf("解有%d种\n", nSolu);
}
