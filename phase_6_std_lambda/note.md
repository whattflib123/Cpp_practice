# Cpp第六章 STL Algorithms + Lambda
> 方便用的工具
## 1. Lambda: 
> 不需要正式定義的函式

- 語法
    ```cpp
    [](int x) { return x > 5; }
    //↑  ↑参数      ↑函式內容(跟普通函式一樣)
    ```
    上下對比同功能函式 
    ```cpp
    bool isBig(int x) {       // 具名函式:要先定義、才能用
        return x > 5;
    }

    // =================================
    // 使用
    auto isBig = [](int x) { return x > 5; };   // auto 推斷型別
    isBig(10);    // true
    isBig(3);     // false
    ```
    `[]` 的使用，可以取得外部變數
    ```cpp
    int threshold = 5;
    auto isBig = [threshold](int x) { return x > threshold; };
    //            ↑ 把外部變數 threshold「捕獲」進來,lambda 內部才能用它
    ```
    - 省去「先定義一個函式、取名字」的步驟, 適合那種用完就丟的邏輯。

## 2. STL Algorithms (`#include <algorithm>`)
> 常用的for loop 算法都被包起來定義好了

並且可以使用 Lambda 作為 STL 的判斷條件

- 例如：
    - `std::sort` —— 排序
        ```cpp
        // 假設輸入 v = {5, 1, 4, 2, 3};
        std::sort(v.begin(), v.end()); // 預設排序，輸出會由小排到大
        // 輸出 {1, 2, 3, 4, 5;

        // 也可傳 lambda 自訂排序規則:
        std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
        // → 由大到小排(lambda 回傳「a 該排在 b 前面嗎」)
        // 輸出 {5, 4, 3, 2, 1};
        ```
    - `std::for_each` —— 對每個元素做一件事
        ```cpp
        std::for_each(v.begin(), v.end(), [](int x) {
            std::cout << x << " ";
        });
        // 等同於 for (int x : v) std::cout << x << " ";
        ```
    - `std::count_if` —— 數有幾個符合條件
        ```cpp
        int n = std::count_if(v.begin(), v.end(), [](int x) {
            return x > 5;
        });
        ```
    - `std::find` / `std::find_if` —— 找東西
       ```cpp
        auto it = std::find(v.begin(), v.end(), 3);          // 找值等於 3 的位置
        auto it2 = std::find_if(v.begin(), v.end(), [](int x) {
            return x % 2 == 0;                                // 找第一個偶數
        });

        if (it != v.end()) {       // 找到了(v.end() 代表「沒找到」)
            std::cout << "found: " << *it;
        }
        ```
    - `std::transform` —— 把每個元素轉換成新的值
    - `std::max_element` / `std::min_element` —— 找最大/最小
    - `std::accumulate` —— 加總(要 #include <numeric>,不是 algorithm)
        ```cpp
        #include <numeric>
        int total = std::accumulate(v.begin(), v.end(), 0);   // 0 是起始值
        ```
- `v.begin(), v.end()` 是什麼:
    ```
    v.begin()  → 指向第一個元素的「東西」(iterator)
    v.end()    → 指向「最後一個元素的下一格」(不是最後一個!是末端標記)
    ```
    代表「整個容器」
    
## 3. 結論：
STL algorithms 是一套「不用自己寫迴圈」就能處理容器的標準函式;lambda 是「現場定義、沒有名字」的小函式,專門配合這些函式用
    
---
    
## 4. 練習: 成績處理

### 情境
你有一份學生分數的 vector,要做兩件事:**統計及格人數**(>= 60 分算及格)、**把所有分數加 5 分當作補考加分**(上限 100,但這題先不管上限,單純 +5)。

### 規格

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> scores = {55, 72, 88, 45, 60, 91, 38};

    // TODO 1: 用 std::count_if + lambda,數出有幾個 >= 60(及格)
    //         結果存進一個叫 passCount 的變數

    // TODO 2: 用 std::transform,把 scores 裡每個分數都 +5
    //         直接改 scores 本身(輸出位置也是 scores.begin())

    std::cout << "及格人數: " << passCount << std::endl;

    for (int s : scores) {
        std::cout << s << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

### 預期輸出
```
及格人數: 4
60 77 93 50 65 96 43
```

