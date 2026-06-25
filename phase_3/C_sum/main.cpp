#include <iostream>

class Vec2{
    public: // 危險的例如指標，才須放在private，這裡public 可以開放給外面使用
        int x;
        int y;

        Vec2(int x, int y){
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

        bool operator==(const Vec2& other) {
            return (x == other.x) && (y == other.y);
        }  
};

int main(){
    Vec2 a(1, 2);
    Vec2 b(1, 2);
    Vec2 c(5, 5);

    if (a == b){
        std::cout << "a 和 b 相等\n";
    }

    if (a == c) {                     
        std::cout << "不會印這行\n";
    }
}

