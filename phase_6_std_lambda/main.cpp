#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> scores = {55, 72, 88, 45, 60, 91, 38};

    // TODO 1: 用 std::count_if + lambda,數出有幾個 >= 60(及格)
    //         結果存進一個叫 passCount 的變數
    int passCount;
    passCount = std::count_if(scores.begin(), scores.end(), 
    [](int x){
        return x >= 60;
    });

    // TODO 2: 用 std::transform,把 scores 裡每個分數都 +5
    //         直接改 scores 本身(輸出位置也是 scores.begin())
    // note: std::transform 的參數順序是:「輸入範圍的起點、輸入範圍的終點、輸出位置的起點、要做的轉換」。
    std::transform(scores.begin(), scores.end(), scores.begin(),
    [](int x){
        return x + 5;
    });

    std::cout << "及格人數: " << passCount << std::endl;

    for (int s : scores) {
        std::cout << s << " ";
    }
    std::cout << std::endl;

    return 0;
}