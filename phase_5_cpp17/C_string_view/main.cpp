#include <iostream>
#include <string_view>
#include <string>

// 寫一個函式 greet,用 string_view 收一個名字,印出 "Hello, 名字!"
// TODO: 函式定義
void greet(std::string_view s){
    std::cout << "Hello, " << s << "!" << std::endl;
}

int main() {
    greet("Alice");                    // 傳字面值 → 不複製
    std::string name = "Bob";
    greet(name);                       // 傳 std::string → 也不複製
    return 0;
}