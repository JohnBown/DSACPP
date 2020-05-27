#include "../stack/stackVector.h"

#include <iostream>
#include <stdio.h>
#include <time.h>

using namespace std;

typedef enum { AVAILABLE, ROUTE, BACKTRACKED, WALL } Status; //迷宫单元状态
//原始可用的, 在当前路径上的, 所有方向均尝试失败后回溯过的, 不可使用的(墙)

typedef enum { UNKNOWN, EAST, SOUTH, WEST, NORTH, NO_WAY } ESWN; //单元的相对领接方向
//未定, 东, 南, 西, 北, 无路可通

inline ESWN nextESWN(ESWN eswn) { return ESWN(eswn + 1); } //依次转至下一邻接方向

struct Cell {                //迷宫格点
    int x, y;                //坐标x, y
    Status status;           //格点状态
    ESWN incoming, outgoing; //进入, 走出方向
};

#define LABY_MAX 24            //最大迷宫尺寸
Cell laby[LABY_MAX][LABY_MAX]; //迷宫

int labySize;    //迷宫尺寸
Cell *startCell; //起点
Cell *goalCell;  //终点

inline Cell *neighbor(Cell *cell) { //查询当前位置的相邻格点
    switch (cell->outgoing) {
        case EAST: return cell + 1;         //向东
        case SOUTH: return cell + LABY_MAX; //向南
        case WEST: return cell - 1;         //向西
        case NORTH: return cell - LABY_MAX; //向北
        default: exit(-1);
    }
}

inline Cell *advance(Cell *cell) { //从当前位置转入相邻格点
    Cell *next;
    switch (cell->outgoing) {
        case EAST:
            next = cell + 1;
            next->incoming = WEST; //向东
            break;
        case SOUTH:
            next = cell + LABY_MAX;
            next->incoming = NORTH; //向南
            break;
        case WEST:
            next = cell - 1;
            next->incoming = EAST; //向西
            break;
        case NORTH:
            next = cell - LABY_MAX;
            next->incoming = SOUTH; //向北
            break;
        default: exit(-1);
    }
    return next;
}

bool labyrinth(Cell Laby[LABY_MAX][LABY_MAX], Cell *s, Cell *t) { //迷宫寻径算法: 从s到t的通路
    if ((AVAILABLE != s->status) || (AVAILABLE != t->status)) {   //退化情况
        return false;
    }
    Stack<Cell *> path; //用栈记录通路
    s->incoming = UNKNOWN;
    s->status = ROUTE;
    path.push(s); //起点入栈
    do {          //从起点出发不断试探&回溯, 直到抵达终点, 或者穷尽所有可能
        Cell *c = path.top(); //检查当前位置(栈顶)
        if (c == t) return true; //若已抵达终点, 则找到一条通路; 否则, 沿尚未试探的方向继续试探
        while (NO_WAY > (c->outgoing = nextESWN(c->outgoing))) { //逐一检查所有方向
            if (AVAILABLE == neighbor(c)->status) break;         //试图找到尚未试探的方向
        }
        if (NO_WAY <= (c->outgoing)) { //若所有方向都已尝试过
            c->status = BACKTRACKED;   //则向后回溯一步
            c = path.pop();
        } else { //否则, 向前试探一步
            path.push(c = advance(c));
            c->outgoing = UNKNOWN;
            c->status = ROUTE;
        }
    } while (!path.empty());
    return false;
}

void randLaby() { //随机一个迷宫
    labySize = LABY_MAX / 2 + rand() % (LABY_MAX / 2);
    for (int i = 0; i < labySize; i++) {
        for (int j = 0; j < labySize; j++) {
            laby[i][j].x = i;
            laby[i][j].y = j;
            laby[i][j].incoming = UNKNOWN;
            laby[i][j].outgoing = UNKNOWN;
            laby[i][j].status = WALL;
        }
    }
    for (int i = 1; i < labySize - 1; i++) {
        for (int j = 1; j < labySize - 1; j++) {
            if (rand() % 4) laby[i][j].status = AVAILABLE;
        }
    }
    startCell = &laby[rand() % (labySize - 2) + 1][rand() % (labySize - 2) + 1];
    goalCell = &laby[rand() % (labySize - 2) + 1][rand() % (labySize - 2) + 1];
    startCell->status = AVAILABLE;
    goalCell->status = AVAILABLE;
}

void displayLaby() { //画迷宫
    char labyy[labySize][labySize];
    for (int i = 0; i < labySize; i++) {
        for (int j = 0; j < labySize; j++) {
            switch (laby[i][j].status) {
                case AVAILABLE: labyy[i][j] = ' '; break;
                case ROUTE: labyy[i][j] = '.'; break;
                case BACKTRACKED: labyy[i][j] = ' '; break;
                case WALL: labyy[i][j] = '#'; break;
                default: exit(-1);
            }
        }
    }
    labyy[startCell->x][startCell->y] = 's';
    labyy[goalCell->x][goalCell->y] = 't';

    for (int i = 0; i < labySize; i++) {
        for (int j = 0; j < labySize; j++) {
            printf("%c", labyy[i][j]);
        }
        printf("\n");
    }
}

int main() { //代码有个bug, 回溯已被标记为ROUTE的路径, 发生: malloc error.
    srand((unsigned int)time(NULL));
    while (true) {
        randLaby();
        displayLaby();
        if (labyrinth(laby, startCell, goalCell)) {
            displayLaby();
            break;
        }
        printf("没找到路径\n");
    }
}