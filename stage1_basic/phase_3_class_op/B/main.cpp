#include <iostream>

class Vec2 {
public:
    int x;
    int y;

    Vec2(int x, int y) {
        this->x = x;      
        this->y = y;
    }

    // operator+:定義「兩個 Vec2 相加」
    Vec2 operator+(const Vec2& other) {
        return Vec2(x + other.x, y + other.y);   // 回傳一個全新的 Vec2
    }

    Vec2 operator-(const Vec2& other){
        return Vec2(x - other.x, y - other.y); 
    }


};

int main(){

    Vec2 a(1, 2);
    Vec2 b(3, 4);
    Vec2 c = a + b;        // 等於 a.operator+(b) → 回傳 Vec2(4, 6)
    std::cout << c.x << ", " << c.y;   // 4, 6

    Vec2 d(5, 8);
    Vec2 e(2, 3);
    Vec2 f = d - e;
    std::cout << f.x << ", " << f.y;   // 應該印 3, 5

    return 0;

}