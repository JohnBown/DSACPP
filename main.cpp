// #include "fibonacci/Fib.h"
#include "vector/vector.h"
#include <bitset>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

int f(int k) { return 0x80 >> (k & 0x07); }

int main() {
    for (int i = 0; i < 16; i++) {
        cout << std::setw(6) << left;
        cout << i << ": " << bitset<8>(f(i)) << endl;
    }
}