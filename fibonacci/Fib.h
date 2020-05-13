#ifndef FIB_H
#define FIB_H

class Fib { // Fibonacci数列类
  private:
    int f, g; // f=fib(k-1)，g=fib(k)。int型，容易数值溢出

  public:
    Fib(int n) { //初始化不小于n的最小Fibonacci项
        f = 1;
        g = 0;
        while (g < n)
            next();
    }
    int get() { return g; } //获取当前Fibonacci项，O(1)时间
    int next() {            //转至下一Fibonacci项，O(1)时间
        g += f;
        f = g - f;
        return g;
    }
    int prev() { //转至上一Fibonacci项，O(1)时间
        f = g - f;
        g -= f;
        return g;
    }
};

#endif