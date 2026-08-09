#include <iostream>
#include <optional>
#include <string>

// 一個查年齡的函式:Alice 是 25,Bob 是 30,其他人查無資料
std::optional<int> findAge(std::string name) {
    // TODO 1: name 是 "Alice" → 回傳 25
    //         name 是 "Bob"   → 回傳 30
    //         其他            → 回傳 std::nullopt
    if (name == "Alice"){return 25; }
    else if (name == "Bob") {return 30;}
    else return std::nullopt;
}

int main() {
    auto a = findAge("Alice");
    // TODO 2: 如果 a 有值 → 印出來;沒有 → 印 "not found"
    if (a.has_value()) {           // 問:裡面有東西嗎?
        std::cout << a.value();    // 有 → 用 .value() 取出來 → 90
    } else {
        std::cout << "not found";
    }

    auto b = findAge("Zoe");
    // TODO 3: 同樣處理 b(Zoe 查無資料,應該印 "not found")
    if (b.has_value()) {           // 問:裡面有東西嗎?
        std::cout << b.value();    // 有 → 用 .value() 取出來 → 90
    } else {
        std::cout << "not found";
    }

    return 0;
}