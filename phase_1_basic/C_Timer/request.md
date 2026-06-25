題 2:Timer —— 自動計時器
情境
你在做模型部署,想知道某段程式碼跑多久(推理延遲是邊緣部署的核心指標)。傳統做法是手動記開始時間、結束時間、相減——但你常常忘記寫結束那行,或中途 return 就漏掉了。
用 RAII 解決:建構子記下開始時間,解構子自動算出經過多久並印出來。這樣只要物件一離開 scope,計時自動結束、自動印——不管你從哪條路離開,絕不會漏。
規格

建構子:記下現在的時間(開始點)。
解構子:算出「從建構到現在」經過幾毫秒,印出 "elapsed: XXX ms"。

就這兩個,沒有 set/get。重點全在「建構開頭、解構結尾」這個 RAII 骨架。
要能跑出這樣
```cpp
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    {
        Timer t;   // ← 計時開始
        // 模擬一段很花時間的工作(睡 100 毫秒)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }   // ← t 離開 scope,解構子自動印 "elapsed: 約100 ms"

    std::cout << "done" << std::endl;
    return 0;
}
```
輸出大概像:
elapsed: 100 ms
done
鷹架(計時的 API 我給你,RAII 骨架你來填)
計時要用 C++ 的 <chrono> 函式庫,語法比較陌生,所以取時間、算差值的那兩行我直接給你,你的任務是把它們放進 RAII 的正確位置(建構子 vs 解構子),並理解為什麼這樣擺。
```cpp
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
```