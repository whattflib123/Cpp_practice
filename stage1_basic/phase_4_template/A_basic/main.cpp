#include <iostream>

// 原本只能比 int:
template <typename T>
T smaller(T a, T b) {
    return (a < b) ? a : b;    // 回傳較小的
}

int main() {
    std::cout << smaller(3, 8) << std::endl;        // 3
    std::cout << smaller(2.5, 1.1) << std::endl;    // 1.1
    std::cout << smaller('z', 'a') << std::endl;    // a
    return 0;
}