
#include <iostream>

class Tensor {
private:
    float* data;
    int tensor_size;

public:
    // 建構子
    Tensor(int size) {
        tensor_size = size;
        // TODO 1: 在 heap 配置 size 個 float,全 0,存進 data
        //   提示:float 陣列歸零 → new float[size]()
        data = new float[size]();
    }

    // 解構子
    ~Tensor() {
        // TODO 2: 若 data 非空 → delete[] 並印 "tensor freed"
        if (data != nullptr){
            // delete data;  // ❌ 配置時用了 new[],釋放要用 delete[]
            delete data[];
            std::cout << "tensor freed" << std::endl;
        }
    }

    // 寫入
    void set(int i, float value) {
        // TODO 3
        data[i] = value
        return data[i];

    }

    // 讀取
    float get(int i) {
        // TODO 4
        return data[i];
    }

    // ↓↓↓ Move 建構子:把 other 的資源過戶過來 ↓↓↓
    Tensor(Tensor&& other) {
        // TODO 5: 三步
        //   (a) data = other 的 data         (抄指標 = 接管那塊記憶體)
        //   (b) tensor_size = other 的 size   (大小也接過來)
        //   (c) other.data = nullptr          (把對方設空 → 過戶後它是空殼)
        //       為什麼要設空?這樣 other 解構時,if 守衛會擋住,不會把這塊釋放掉
        data = other.data;
        tensor_size = other.tensor_size;
        other.data = nullptr;

    }

    // 禁止 copy(跟 unique_ptr 一樣的標準寫法,直接給你)
    Tensor(const Tensor&) = delete;
    Tensor& operator=(const Tensor&) = delete;
};

Tensor make_tensor() {
    Tensor t(1000000);
    t.set(0, 3.14f);
    return t;
}

int main() {
    Tensor result = make_tensor();
    std::cout << result.get(0) << std::endl;
    return 0;
}


Tensor make_tensor() {
    Tensor t(1000000);     // 在函式裡造一塊一百萬 float 的 tensor
    t.set(0, 3.14f);
    return t;              // 把它「交棒」出去(這裡會觸發 move,不是複製!)
}

int main() {
    Tensor result = make_tensor();   // result 接管那塊資料,沒有任何複製發生
    std::cout << result.get(0) << std::endl;   // 3.14
    return 0;
}   // result 解構 → "tensor freed"(只印一次)