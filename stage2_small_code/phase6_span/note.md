# Stage 6 — `std::span` 借用語意

## 這關學什麼

`std::span` 是一個**非擁有的視圖（non-owning view）**，同時帶著指標和長度。
解決 Stage 2 留下的問題：裸指標傳入函式後長度消失。

| | 擁有資料？ | 帶長度？ | heap 配置？ |
|---|---|---|---|
| `int*` | ❌ | ❌ | 看情況 |
| `std::vector<int>` | ✅ | ✅ | ✅ |
| `std::span<int>` | ❌ | ✅ | ❌ |

**核心概念**：span 是借用，不是擁有。它的生命週期不能超過被它指向的資料。

---

## 語法參考卡

```cpp
#include <span>

// 建立 span
std::span<int>       s1 = arr;           // from raw array（自動推長度）
std::span<int>       s2 = vec;           // from std::vector
std::span<const int> s3 = arr;           // 唯讀視圖
std::span<int, 4>    s4 = arr;           // 靜態長度（編譯期確定）

// 常用操作
s.size()                // 元素個數
s.data()                // 第一個元素的指標
s[i]                    // 存取元素（沒有 bounds check，debug 用 .at() 或自己加）
s.subspan(offset, len)  // 子視圖，從 offset 開始取 len 個元素
s.first(n)              // 前 n 個元素
s.last(n)               // 後 n 個元素
```

---

## 微題階梯

### 題 6-1：裸指標 vs span

寫兩個版本的 `print_arr`：

```cpp
// 版本 A：傳統裸指標
void print_raw(const int* data, int len) { ... }

// 版本 B：std::span
void print_span(std::span<const int> s) { ... }
```

在 main 建一個 `int arr[5] = {10, 20, 30, 40, 50}`，兩個版本都呼叫，印出元素。
在函式內各印一次 `s.size()`（或 `len`），感受 span 把長度帶進來了。

**觀察重點**：版本 B 呼叫時不需要傳長度，span 從陣列型別自動推導。

---

### 題 6-2：從 `std::vector` 建 span，驗證零複製

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};
std::span<int> s = vec;
```

- 印 `vec.data()` 和 `s.data()`，確認是同一個位址
- 透過 `s[0] = 999` 修改，再印 `vec[0]`，確認 span 真的是視圖而非複製

---

### 題 6-3：`std::span<const int>` 唯讀借用

把題 6-2 的 `std::span<int>` 改成 `std::span<const int>`，
再試圖 `s[0] = 999` → 應該編譯錯誤。

**重點**：`const` 在 span 的 element type 上，不是在 span 本身。

---

### 題 6-4：Lifetime bug — dangling span

```cpp
std::span<int> make_span() {
    std::vector<int> v = {1, 2, 3};
    return v;   // v 在函式結束時銷毀
}
int main() {
    auto s = make_span();
    std::cout << s[0] << '\n';  // UB: dangling
}
```

編譯 + 執行，觀察 ASan 或 sanitizer 報什麼。

---

### 題 6-5：`subspan` — 模擬 TensorRT output buffer 後處理

```cpp
// 假設 TensorRT 把推論結果寫進一塊連續記憶體
// layout: [box0_x, box0_y, box0_w, box0_h, box0_score,
//           box1_x, box1_y, ...]
// 每個 box 5 個 float
std::vector<float> output(25);  // 5 個 box
// 用 subspan 取出每個 box 的 5 個值，印出 score（第 4 個）
```

---

## 面試常考追問

- `std::span` vs `const std::vector<int>&`：什麼時候用哪個？
- `std::span` 的 lifetime 風險是什麼？怎麼避免？
- dynamic extent vs static extent（`std::span<int>` vs `std::span<int, N>`）差在哪？
- `std::string_view` 是同一個概念嗎？
