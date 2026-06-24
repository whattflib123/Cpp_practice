#include <iostream>
#include <map>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> words = {"apple", "banana", "apple", "cherry", "banana", "apple"};

    std::map<std::string, int> count;   // key=單字, value=出現次數

    // TODO: 跑過 words,每個單字在 map 裡 +1

    for (std::string w : words){
        count[w] = count[w] + 1; // 這是map 的特性 如果一開始沒有這格key，會自動建立key並將值初始化為0，也就是一開始的count[w] = 0
    }

    // 印出每個單字和它的次數
    for (auto& pair : count) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}