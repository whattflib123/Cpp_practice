好,收官題 **Tensor**。這題把 **move** 加回戰場,而且情境直接對準你的部署職涯——這就是你未來程式碼裡「模型輸出在管線間流動」的縮小版。我先用一句話定調這題在練什麼:

> 前面 SharedBuffer 教你「複製時共享、計數+1」。這題教你相反的另一條路:當一塊大資料**只想轉移、不想複製、也不要共享**——直接把所有權整個搬走,原主人變空殼。這就是 move,大資料在管線間流動的標準做法。

---

## 題 4:`Tensor` —— 支援 move 的資料容器

### 情境
你管著一塊 float 陣列(想像是一層的特徵輸出 / 一塊 tensor,可能幾百萬個 float)。這塊資料要從一個函式「交棒」給下一個處理階段。複製它=災難(幾百萬個 float 逐個抄),所以你要讓它支援 **move**:把資源整個過戶,瞬間完成。

### 規格
1. **建構子**:接收 `size`,在 heap 配置 size 個 float(全 0),記住 size。
2. **解構子**:釋放;印 `"tensor freed"`。**注意**:被 move 搬空的物件 data 是 nullptr,要避免印或重複釋放(用你 SharedBuffer / unique_ptr 學過的 `if` 守衛)。
3. **`set(i, value)` / `get(i)`**:讀寫第 i 個 float。
4. **Move 建構子**:把 other 的資源**過戶**過來,other 設空(data = nullptr）。
5. **禁止 copy**:這塊大資料不准被複製(跟 unique_ptr 同精神)。

### 要能跑出
```cpp
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
```

### 鷹架
```cpp
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
    }

    // 解構子
    ~Tensor() {
        // TODO 2: 若 data 非空 → delete[] 並印 "tensor freed"
    }

    // 寫入
    void set(int i, float value) {
        // TODO 3
    }

    // 讀取
    float get(int i) {
        // TODO 4
    }

    // ↓↓↓ Move 建構子:把 other 的資源過戶過來 ↓↓↓
    Tensor(Tensor&& other) {
        // TODO 5: 三步
        //   (a) data = other 的 data         (抄指標 = 接管那塊記憶體)
        //   (b) tensor_size = other 的 size   (大小也接過來)
        //   (c) other.data = nullptr          (把對方設空 → 過戶後它是空殼)
        //       為什麼要設空?這樣 other 解構時,if 守衛會擋住,不會把這塊釋放掉
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
```

### 提示(卡住再看)
- TODO 1:`data = new float[size]();`(注意是 `float` 不是 int,方括號 + 結尾 `()` 歸零)
- TODO 2:`if (data != nullptr) { delete[] data; std::cout << "tensor freed\n"; }`
- TODO 3:`data[i] = value;`
- TODO 4:`return data[i];`
- TODO 5:照註解 (a)(b)(c) 三行,跟你 unique_ptr 的 move 建構子幾乎一樣,只是多搬一個 `tensor_size`

### 寫的時候想這件事(這題的靈魂)
`return t;` 那行——`t` 是函式裡的區域變數,函式結束它就要死了。C++ 看到「你要回傳一個即將死亡的物件」,會自動選擇 **move** 而不是 copy(把 t 的資源搬給 result,而不是抄一份)。這正是你學 move 時那條準則的體現:**「之後不再用了」的物件,就 move。** 而且因為你**禁止了 copy**,這塊大資料**根本不可能被複製**——編譯器只能 move,從型別層面保證了效能。

TODO 1~5 填完貼給我。前四個是你閉著眼睛都會的複習,重點在 TODO 5 的 move 建構子——但你 unique_ptr 已經寫過幾乎一樣的東西了,試試看。