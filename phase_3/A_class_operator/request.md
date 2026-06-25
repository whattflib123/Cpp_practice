**驗收題——把 `operator[]` 加進你已經會的 Buffer:**

我給你一個精簡版 Buffer(建構、解構、size 都幫你寫好),**你只要加一個 `operator[]`**,讓 main 裡的 `buf[i]` 能用:

```cpp
#include <iostream>

class Buffer {
private:
    int* data;
    int size;
public:
    Buffer(int s) {
        size = s;
        data = new int[s]();
    }
    ~Buffer() {
        if (data != nullptr) {
            delete[] data;
        }
    }

    // TODO: 加一個 operator[],回傳第 i 格的「參考」
    //   提示:回傳型別 int&,函式名 operator[],參數 int i,body 是 return data[i];

};

int main() {
    Buffer buf(5);
    buf[0] = 100;              // 用 [] 寫入
    buf[4] = 999;
    std::cout << buf[0] << std::endl;   // 100
    std::cout << buf[4] << std::endl;   // 999
    return 0;
}
```

