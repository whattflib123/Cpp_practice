#include <iostream>

enum class Direction { North, South, East, West };

void printDirection(Direction d) {
    // TODO: 用 switch,每個 case 印出對應中文,別忘了 break
    switch (d)
    {
    case Direction::North:
        /* code */
        std::cout << "北";
        break;
    case Direction::South:
        /* code */
        std::cout << "南";
        break;
    case Direction::East:
        /* code */
        std::cout << "東";
        break;
    case Direction::West:
        /* code */
        std::cout << "西";
        break;  
    default:
        break;
    }
}

int main() {
    printDirection(Direction::North);   // 北
    printDirection(Direction::East);    // 東
    return 0;
}