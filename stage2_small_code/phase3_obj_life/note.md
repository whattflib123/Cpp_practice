# Stage 3 — 物件生命週期

## 為什麼這關重要

SRAM perception pipeline 資料流：

```
Camera driver → Frame buffer → Preprocessor → TensorRT inference
```

每個箭頭 = 把物件傳給下一個函式。C++ 傳物件有三種結果：

| 方式 | 成本 | 說明 |
|---|---|---|
| 複製（copy） | 高 | 做一份新的，原本繼續活 |
| 搬移（move） | 低 | 資源轉走，原本變空殼 |
| 借用（const&） | 0 | 唯讀，不能存活超過原物件 |

4K YUV frame = 約 8 MB。內圈每幀複製一次就是每幀 8 MB 寫入。
**搞錯傳值方式，就是在 inference 迴圈裡每幀多複製 8 MB。**

面試常考：「你怎麼把 frame 傳給 inference function？為什麼？」

---

## Stage 3 核心目標

**用肉眼看到複製/搬移/解構發生的那一行。**

概念再熟，不如讓程式自己印出來：

```
ctor  #1       ← 物件誕生
copy  #1       ← 複製發生（這行是關鍵）
dtor  #1       ← 複本離開 scope，死亡
dtor  #1       ← 原物件離開 scope，死亡
```

這個 log 技巧在 debug 效能問題時真的會用到（`RAII wrapper` + 計數器）。

建 stage2_small_code/phase3_obj_life/main.cpp，實作 Tracker class：

- 成員：int id
- 三個 special member 各印一行 log：
- ctor：ctor  #<id>
- copy ctor：copy  #<id>
- dtor：dtor  #<id>

main 只寫這一行：
Tracker a(1);

預期輸出：
ctor  #1
dtor  #1

copy ctor 這題不會出現，但要寫出來，下一題才能觀察到它。

編譯：
g++ -std=c++20 -Wall -Wextra -g -fsanitize=address main.cpp -o main &&
./main




---

## Special Member Functions 語法參考卡

```cpp
class Dog {
    std::string name;
public:
    Dog(std::string n) : name(n) {            // 一般建構子（ctor）
        std::cout << "ctor  " << name << '\n';
    }
    Dog(const Dog& other) : name(other.name) { // 拷貝建構子（copy ctor）
        std::cout << "copy  " << name << '\n';
    }
    ~Dog() {                                   // 解構子（dtor）
        std::cout << "dtor  " << name << '\n';
    }
};
```

觸發時機：

| 情況 | 觸發 |
|---|---|
| `Dog a("Rex");` | ctor |
| `Dog b = a;` 或 `Dog b(a);` | copy ctor |
| `void f(Dog d)` 被呼叫 | copy ctor（傳入那一刻） |
| 離開 `}` | dtor（後建構的先解構，LIFO） |
| `return localDog;` | 通常觸發 RVO，**不**觸發 copy |

---

## 微題階梯

### 題 3-1：基本 ctor / dtor
只建一個物件，確認 log 出現順序。

```cpp
Tracker a(1);
// 預期：ctor #1 → dtor #1
```

### 題 3-2：by-value 傳參觸發 copy
```cpp
void inspect(Tracker t) { ... }
inspect(a);
// 預期：copy ctor 在 inspect 被呼叫時觸發
//       dtor 在 inspect 返回時觸發（複本死亡）
```

### 題 3-3：by const& 消滅 copy
```cpp
void inspect(const Tracker& t) { ... }
inspect(a);
// 預期：沒有 copy ctor，dtor 也少一次
```

### 題 3-4：RVO — 回傳 local object 不複製
```cpp
Tracker make() {
    Tracker t(99);
    return t;
}
Tracker x = make();
// 預期：只看到一次 ctor，沒有 copy（編譯器直接在呼叫端建構）
```

### 題 3-5：`std::move` 初探（Stage 4 深挖）
```cpp
Tracker b = std::move(a);
// 預期：move ctor 觸發（需要先宣告 move ctor）
```

---

## 關鍵觀念（練完應能回答）

1. **copy ctor 和普通 ctor 簽名差在哪？**
   普通：`Tracker(int id)`；copy：`Tracker(const Tracker& other)`。
   參數是同型別的 const reference。

2. **dtor 順序為什麼是反過來的？**
   Stack-like：後建構的先解構（LIFO）。
   同一 scope 內 `a` 先建、`b` 後建 → `b` 先 dtor、`a` 後 dtor。

3. **RVO 是什麼？編譯器何時會做？**
   Return Value Optimization：直接在呼叫端的記憶體空間建構物件，不透過複製。
   條件：一個函式、一條 return、回傳同一個 local 物件（named → NRVO）。
   `return std::move(x)` 會**關掉** NRVO，別這樣寫。

4. **什麼時候用 by-value？什麼時候用 const&？**
   唯讀用途 → `const&`（零成本）。
   需要留一份 → by-value（讓 caller 決定 copy 或 move）。
   要改原物件 → `&`。

---

## 面試常考追問

- 「你能解釋你的 class 裡發生了幾次拷貝嗎？」
- 「`return` 一個 local vector，會複製嗎？」（NRVO）
- 「`Tracker b = std::move(a)` 之後，`a` 還能用嗎？」（moved-from state）
- 「copy ctor 和 assignment operator 有什麼不同？」（Stage 4 深挖）
