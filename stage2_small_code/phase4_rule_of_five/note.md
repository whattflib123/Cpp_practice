# Stage 4 — 拷貝 vs 搬移：手刻 Rule of Five

## 為什麼這關重要

Stage 3 的 `Tracker` 只有 `int id`，move 沒有「真的搬走東西」。
Stage 4 讓 class 持有一塊 heap buffer，這時 copy/move 的成本差距才真的看得見。

SRAM 場景：

```
Camera driver
  ↓ 每幀產生一個 Frame（含 heap 上的 8 MB YUV buffer）
Frame queue（std::vector<Frame>）
  ↓ push_back / emplace_back
Inference thread
```

`std::vector<Frame>` 擴容時要把所有元素搬到新位置。
- move ctor 標 `noexcept` → vector 選 move（O(1) per element）
- 沒標 `noexcept` → vector 選 copy（O(n) per element，8 MB × N 幀）

面試常考：「你的 Frame class 放進 vector，擴容時發生了幾次拷貝？」

---

## Rule of Five 是什麼

當 class 自己管理資源（裸指標、file descriptor、GPU memory 等），
編譯器自動生成的版本**不夠用**，必須自己寫五個：

| # | 名稱 | 觸發時機 |
|---|---|---|
| 1 | Destructor | 離開 scope |
| 2 | Copy constructor | `Frame b = a;` |
| 3 | Copy assignment | `b = a;`（b 已存在） |
| 4 | Move constructor | `Frame b = std::move(a);` |
| 5 | Move assignment | `b = std::move(a);`（b 已存在） |

少寫任何一個，就會有 **double free** 或 **memory leak**。

---

## 語法參考卡（用 `Buffer` 舉例，不是練習答案）

```cpp
class Buffer {
    char* data;
    int   sz;
public:
    // 1. ctor
    Buffer(int n) : data(new char[n]), sz(n) {}

    // 2. dtor
    ~Buffer() { delete[] data; }

    // 3. copy ctor — deep copy
    Buffer(const Buffer& other) : data(new char[other.sz]), sz(other.sz) {
        std::copy(other.data, other.data + sz, data);
    }

    // 4. copy assignment
    Buffer& operator=(const Buffer& other) {
        if (this == &other) return *this;   // self-assign guard
        delete[] data;
        sz   = other.sz;
        data = new char[sz];
        std::copy(other.data, other.data + sz, data);
        return *this;
    }

    // 5. move ctor — steal, then null source
    Buffer(Buffer&& other) noexcept : data(other.data), sz(other.sz) {
        other.data = nullptr;
        other.sz   = 0;
    }

    // 6. move assignment
    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) return *this;
        delete[] data;
        data       = other.data;
        sz         = other.sz;
        other.data = nullptr;
        other.sz   = 0;
        return *this;
    }
};
```

`noexcept` 告訴 `std::vector`：「move 不會丟例外，可以安心用 move 代替 copy。」

---

## 微題階梯

### 題 4-1：dtor only → 觀察 double free

寫 `Frame` class：
- 成員：`char* buf`、`int size`
- ctor：`new char[size]`，印 `alloc #<id>`
- dtor：`delete[] buf`，印 `free  #<id>`
- **故意不寫** copy ctor

```cpp
Frame a(1, 64);   // alloc #1
Frame b = a;      // 複製（用預設 copy ctor，shallow copy）
// 離開 scope → b.dtor → a.dtor → double free!
```

預期：ASan 報 `double-free`。

---

### 題 4-2：加 copy ctor — deep copy

自己寫 copy ctor，`new` 一塊新的 `char[]`，用 `std::copy` 複製內容。

```cpp
Frame b = a;   // 現在 b 有自己的 buf
// 兩個 dtor，各自 delete 各自的 buf → 正確
```

預期：`alloc #1 / alloc #1(copy) / free #1 / free #1`，不再崩潰。

---

### 題 4-3：加 copy assignment

```cpp
Frame a(1, 64);
Frame b(2, 64);
b = a;   // copy assignment（b 已存在）
```

注意：`b` 原本有自己的 `buf`，assign 前要先 `delete[] buf`，
否則舊的那塊 heap 沒人管 → leak。

---

### 題 4-4：加 move ctor

```cpp
Frame b = std::move(a);
// b 接管 a.buf，a.buf = nullptr
// a.dtor → delete nullptr（安全，no-op）
```

關鍵：move ctor 裡**手動** `other.buf = nullptr`，
否則 a.dtor 和 b.dtor 都 `delete` 同一塊 → double free。

---

### 題 4-5：加 move assignment + noexcept

```cpp
Frame a(1, 64);
Frame b(2, 64);
b = std::move(a);   // move assignment
```

同題 4-3 的 guard：先 `delete[] buf` 清掉 b 舊有的，
再把 a 的 buf 接過來，a.buf = nullptr。

最後把 move ctor 和 move assignment 都加上 `noexcept`，
觀察 `std::vector<Frame>` push_back 時是否選 move。

---

## 關鍵觀念（練完應能回答）

1. **shallow copy 為什麼會 double free？**
2. **copy ctor 和 copy assignment 差在哪？**（一個是建構新物件，一個是覆寫已存在的）
3. **move ctor 裡為什麼一定要 `other.buf = nullptr`？**
4. **`noexcept` 對 `std::vector` 有什麼影響？**

---

## 面試常考追問

- 「Rule of Three 和 Rule of Five 差在哪？」（C++11 加了 move）
- 「Rule of Zero 是什麼？什麼情況下用？」（讓 RAII wrapper 管資源，自己的 class 不手寫五個）
- 「copy assignment 裡為什麼要 self-assign guard？」
- 「你說 move 比 copy 快，能定量說快多少？」（指標 swap O(1) vs memcpy O(n)）
