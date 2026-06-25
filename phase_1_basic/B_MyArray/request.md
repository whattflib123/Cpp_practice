先做題 1,完整題目如下:
題 1:MyArray
情境
你要在 heap 上管理一排 int(想像是一串感測器讀數)。跟上一題一樣用 RAII 自動釋放,但這次是陣列,還要能安全地讀寫每個位置。
規格

建構子:接收一個大小 size,在 heap 配置這麼多個 int(全部初始化為 0)。
解構子:自動釋放,印出 "array freed"。
get(i):回傳第 i 個元素。
set(i, value):把第 i 個位置設成 value。
size():回傳陣列大小。

要能跑出
```cpp
int main() {
    MyArray arr(5);          // 5 個 int,全 0
    arr.set(0, 100);
    arr.set(4, 999);
    std::cout << arr.get(0) << std::endl;   // 100
    std::cout << arr.get(4) << std::endl;   // 999
    std::cout << arr.size() << std::endl;   // 5
    return 0;
}   // 印出 "array freed"
```
鷹架(填 TODO,這次只有你學過的東西)
```cpp
#include <iostream>

class MyArray {
private:
    int* data;      // 指向 heap 上那排 int
    int arr_size;   // 記住大小

public:
    // 建構子
    MyArray(int size) {
        arr_size = size;
        // TODO 1: 在 heap 配置 size 個 int,全部初始化為 0,存進 data
        //   提示:配置陣列並歸零,語法是  new int[size]()
        //         (結尾那個 () 會把每個元素設成 0)
    }

    // 解構子
    ~MyArray() {
        // TODO 2: 釋放 data(陣列要用哪個 delete?),印出 "array freed"
    }

    // 設定第 i 個位置的值
    void set(int i, int value) {
        // TODO 3: 把 data 的第 i 個元素設成 value
        //   提示:指標可以像陣列一樣用 data[i] 存取
    }

    // 取得第 i 個位置的值
    int get(int i) {
        // TODO 4: 回傳 data 的第 i 個元素
    }

    // 回傳大小
    int size() {
        // TODO 5: 回傳 arr_size
    }
};

int main() {
    MyArray arr(5);
    arr.set(0, 100);
    arr.set(4, 999);
    std::cout << arr.get(0) << std::endl;   // 100
    std::cout << arr.get(4) << std::endl;   // 999
    std::cout << arr.size() << std::endl;   // 5
    return 0;
}
```