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
        data = new int[size]();
    }

    // 解構子
    ~MyArray() {
        // TODO 2: 釋放 data(陣列要用哪個 delete?),印出 "array freed"
        if (data != nullptr){

            // delete data;     // ❌(就算名字修對,delete 也不對)
            delete[] data;    // ✓ 陣列配對方括號
            std::cout << "array freed" << std::endl;
        }
    }

    // 設定第 i 個位置的值
    void set(int i, int value) {
        // TODO 3: 把 data 的第 i 個元素設成 value
        //   提示:指標可以像陣列一樣用 data[i] 存取
        data[i] = value;
    }

    // 取得第 i 個位置的值
    int get(int i) {
        // TODO 4: 回傳 data 的第 i 個元素
        return data[i];
    }

    // 回傳大小
    int size() {
        // TODO 5: 回傳 arr_size
        // return data.size();   // ❌
        return arr_size;      // ✓
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