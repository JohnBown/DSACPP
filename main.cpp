// #include "fibonacci/Fib.h"
#include "vector/vector.h"
// #include <bitset>
// #include <iomanip>
#include <iostream>
// #include <string>

using namespace std;

// int f(int k) { return 0x80 >> (k & 0x07); }

int main() {
    Vector<int> v;
    v.insert(5);
    v.insert(6);
    v.insert(2);
    v.insert(7);
    v.insert(1);
    v.insert(4);
    v.insert(3);

    for (int i = 0; i < v.size(); i++) {
        cout << v[i] << endl;
    }

    v.sort();

    cout << "====" << endl;

    for (int i = 0; i < v.size(); i++) {
        cout << v[i] << endl;
    }
}