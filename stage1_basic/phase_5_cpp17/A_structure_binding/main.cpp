#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> scores = {
        {"Alice", 90},
        {"Bob", 85},
        {"Cathy", 95}
    };

    // TODO: 用結構化綁定遍歷,印出 "名字: 分數"
    //   (用 for (auto& [n, s] : scores) 這種寫法,不要用 .first/.second)
    for (auto& [n, s] : scores){
        std::cout << n << ": " << s << std::endl;
    }
    return 0;
}