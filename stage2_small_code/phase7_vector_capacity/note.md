# Stage 7 — `std::vector` capacity vs size

## 這關學什麼

`std::vector` 在 detection queue 裡是最常見的資料結構。
搞懂 capacity / size / reallocation，才能解釋「為什麼 push_back 有時候慢、有時候快」。

| 概念 | 意思 |
|---|---|
| `size()` | 目前存了幾個元素 |
| `capacity()` | 目前配置了多少空間（不一定全用） |
| `push_back` | size+1；若 size == capacity → reallocation（慢），否則 O(1)（快） |
| reallocation | 新配置 capacity×2 的空間，把舊元素全移過去，釋放舊空間 |

**Amortized O(1)**：大多數 push_back 是 O(1)，偶爾觸發 O(n) reallocation，
平均下來每個元素的成本仍是 O(1)。

---

## 語法參考卡

```cpp
std::vector<int> v;
v.reserve(100);       // 預先配置 capacity，不改 size
v.push_back(42);      // size+1；capacity 不夠才 realloc
v.size();             // 目前元素個數
v.capacity();         // 目前配置空間
v.shrink_to_fit();    // 把 capacity 縮回 size（hint，不保證）

// iterator invalidation
auto it = v.begin();
v.push_back(99);      // 若觸發 realloc → it 變 dangling！
```

---

## 微題階梯

### 題 7-1：觀察 capacity 成長

```cpp
std::vector<int> v;
for (int i = 0; i < 20; i++) {
    v.push_back(i);
    std::cout << "size=" << v.size()
              << " cap=" << v.capacity() << '\n';
}
```

觀察 capacity 什麼時候翻倍（不同 STL 實作可能是 ×1.5 或 ×2）。

---

### 題 7-2：`reserve` 消除 reallocation

把題 7-1 加一行 `v.reserve(20)` 在迴圈前，重跑，比較 capacity 變化。

---

### 題 7-3：iterator invalidation

```cpp
std::vector<int> v = {1, 2, 3};
auto it = v.begin();
v.push_back(4);  // 可能 realloc
std::cout << *it << '\n';  // UB？
```

先猜答案，再跑 ASan 看結果。

---

### 題 7-4：`reserve` 防止 iterator invalidation

題 7-3 加 `v.reserve(10)` 讓 capacity 夠大，再跑，確認 `*it` 安全。

---

## 面試常考追問

- push_back amortized O(1) 的數學是什麼？（為什麼 ×2 而不是 +1？）
- `resize` vs `reserve` 差在哪？
- `std::vector` reallocation 用 move 還是 copy？什麼條件決定？（連到 Stage 4 `noexcept`）
- iterator invalidation 的完整規則是什麼？哪些操作安全？
