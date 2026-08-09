# 筆記

## 1. `std::vector<Obj>`:
表示在記憶體 Heap 上 配置一組連續的記憶體， 把每個物件 Obj 連續排放

```
struct Detection {
    int   id;
    float score;
    float x, y, w, h;
};

記憶體位址遞增 →
┌─────────────┬─────────────┬─────────────┬─────────────┐
│ Detection[0]│ Detection[1]│ Detection[2]│ Detection[3]│
│ id,score,   │ id,score,   │ id,score,   │ id,score,   │
│ x,y,w,h     │ x,y,w,h     │ x,y,w,h     │ x,y,w,h     │
└─────────────┴─────────────┴─────────────┴─────────────┘
```

## 2. brace init (大括號初始化 / uniform initialization): 
> 基本的初始化大概三種: `=`, `()`, `{}`
> **brace init {} 應優先被使用**

## 核心差異表

| 特性 | `=` (copy init) | `()` (direct init) | `{}` (brace init) |
|---|---|---|---|
| **Narrowing conversion** | 允許,悄悄截斷 | 允許,悄悄截斷 | **禁止**,編譯錯誤 |
| **Most vexing parse** | 不會發生 | **會發生**(見下方範例) | 不會發生 |
| **呼叫 `explicit` constructor** | ❌ 不行 | ✅ 可以 | ✅ 可以 |
| **STL 容器裡 `initializer_list` 優先權** | 不適用 | 不會誤觸發 | **會**優先選 `initializer_list` 版本 |
| **Aggregate(無 constructor 的 struct)** | ❌ 不行 | ❌ 不行 | ✅ 可以 |

```cpp
int x = 5;    // ① copy initialization
int y(5);     // ② direct initialization
int z{5};     // ③ brace initialization (uniform init, C++11+)
```