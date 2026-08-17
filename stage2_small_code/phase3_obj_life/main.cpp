#include <iostream>

class Tracker {
    int id;
public:
    Tracker(int id) : id(id) {
        std::cout << "ctor  #" << id << '\n';   // 用參數 id 或 this->id 都可以，值一樣
    }
    Tracker(const Tracker& other) : id(other.id) {
        std::cout << "copy  #" << id << '\n';   // 這裡只能用 this->id（成員），因為沒有同名參數
    }
    Tracker(Tracker&& other) : id(other.id) {
        std::cout << "move  #" << id << '\n';
    }
    ~Tracker() {
        std::cout << "dtor  #" << id << '\n';
    }
};
void inspect(const Tracker& t) {
    std::cout << "inside inspect\n";
}
Tracker make() {
    Tracker t(99);
    return t;
}
int main() {
    Tracker a(1);
    // Tracker b = a;
    // Tracker b(2);

    // inspect(a);
    // Tracker x = make();

    Tracker b = std::move(a);


    return 0;
}
