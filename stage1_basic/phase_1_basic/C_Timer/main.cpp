#include <iostream>
#include <chrono>
#include <thread>

class Timer {
private:
    // 用來存「開始時間」的成員變數,型別有點長,照抄即可
    std::chrono::high_resolution_clock::time_point start_time;

public:
    // 建構子
    Timer() {
        // TODO 1: 記下現在的時間,存進 start_time
        //   取得現在時間的語法(直接用):
        //   std::chrono::high_resolution_clock::now()
        start_time = std::chrono::high_resolution_clock::now();
    }

    // 解構子
    ~Timer() {
        // TODO 2: 
        //   (a) 再取一次現在時間(結束點)
        //   (b) 算出經過的毫秒數
        //   (c) 印出 "elapsed: XXX ms"
        //
        //   算毫秒差的語法(直接用),假設結束時間變數叫 end_time:
        //   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        //                       end_time - start_time
        //                   ).count();
        //   這個 duration 就是毫秒數(整數)
        auto end_time = std::chrono::high_resolution_clock::now(); // 我沒加這行
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << "elapsed: "<< duration <<" ms" << std::endl;
    }
};

int main() {
    {
        Timer t;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "done" << std::endl;
    return 0;
}