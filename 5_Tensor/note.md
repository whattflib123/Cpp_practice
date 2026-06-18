# C++ 教材：move 是怎麼被「自動呼叫」的？

> 主題：lvalue / rvalue、`&&`、move 建構子的觸發機制
> 前置知識：建構子、指標、move 的基本概念

---

## 0. 一句話總結（先看結論，再看推導）

> 編譯器靠**參數型別**配對該呼叫哪個建構子。
> `&&` 是一種「**專門接即將消失的值**」的參數型別，
> 所以當來源是即將消失的東西時，就自動配對到 `move 建構子`。

---

## 1. 先複習：編譯器靠「參數型別」選建構子

一個 class 可以有很多同名建構子，編譯器看**你給什麼參數**來決定走哪個。

```cpp
Tensor(int size)              // 給 int        → 走這個（從零創建）
Tensor(const Tensor& other)   // 給現成 Tensor → 走這個（複製）
Tensor(Tensor&& other)        // 給「即將消失的」Tensor → 走這個（move）
```

關鍵在第三個的 `Tensor&&`：
- `&`（一個）和 `&&`（兩個）是**兩種不同的參數型別**
- 編譯器把它們當成不同建構子來配對

所以真正的問題是：**什麼樣的來源會被配對到 `&&` 這個？**

---

## 2. 核心觀念：lvalue vs rvalue

C++ 把「值」分成兩大類：

| 類別 | 白話 | 例子 |
|------|------|------|
| **lvalue（左值）** | 有名字、之後還會用到 | `int x = 5;` 的 `x` |
| **rvalue（右值）** | 沒名字 / 即將消失的暫時值 | `5`、`x + 1` 的結果、**即將被 return 而死亡的區域變數** |

### 配對規則（整段的鑰匙）

```
Tensor&   （一個 &）  → 只能接 lvalue
Tensor&&  （兩個 &）  → 專門接 rvalue
```

> `&&` 的存在意義就是：
> 「我專門接那些**即將消失、可以放心搬空**的東西。」

---

## 3. 為什麼 `return t;` 會自動觸發 move？

```cpp
Tensor make_tensor() {
    Tensor t(1000000);
    return t;          // ★ move 建構子在這裡被呼叫
}
```

推理過程（編譯器在做的事）：

```
return t;
  ↓ t 是區域變數，return 之後馬上要死
  ↓ 編譯器判定：t 屬於「即將消失」那類 → 當成 rvalue
  ↓ 找「能接 rvalue」的建構子
  ↓ Tensor&& 正好就是專門接 rvalue 的
  ↓ 選中 Tensor(Tensor&& other) ← move 建構子！
```

**重點：編譯器不是「讀懂你想 move」，而是機械地套規則：**
來源是即將消失的（rvalue）→ 配對到參數型別 `&&` 的建構子。

`&&` 就是 move 建構子的「識別標誌」。

---

## 4. 那 `std::move` 是什麼？

當變數**有名字**（lvalue）時，編譯器預設**不會**自動 move（怕你之後還要用）：

```cpp
Tensor a(100);
Tensor b = a;              // a 是 lvalue → 預設走「複製」（這裡被 = delete 擋下）
Tensor b = std::move(a);   // 手動把 a「標記成 rvalue」→ 走 move
```

`std::move(a)` 做的事：
- **它本身不搬任何東西**
- 它只是把一個 lvalue **「假裝成」rvalue**（貼上「我可被搬走」的標籤）
- 一旦被標記成 rvalue，配對規則就會選中 `&&` 那個 move 建構子

### 兩種觸發 move 的情況，本質相同

```
return t;        → 編譯器「自動」判定 t 是即將消失的 rvalue → 選 &&
std::move(a);    → 你「手動」把 a 標記成 rvalue              → 選 &&
```

兩者最後都是：**讓來源變成 rvalue → 配對到 `Tensor&&` 建構子**。
差別只在「自動 vs 手動」。

---

## 5. move 建構子內部在做什麼

```cpp
Tensor(Tensor&& other) {
    data = other.data;               // (a) 接管那塊記憶體（抄指標，不複製內容）
    tensor_size = other.tensor_size; // (b) 大小也接過來
    other.data = nullptr;            // (c) 把對方設空 → 它變空殼
}
```

- (a) 只抄一根**指標**，不管那塊記憶體裡是 1 個還是一百萬個 float，都是瞬間完成
- (c) 設 `nullptr` 的目的：讓 `other` 解構時被 `if (data != nullptr)` 守衛擋住，
  **不會把這塊已經過戶出去的記憶體重複釋放**（避免 double free）

---

## 6. 怎麼「親眼看到」它被呼叫？

在 move 建構子裡加一行 print：

```cpp
Tensor(Tensor&& other) {
    std::cout << "move called\n";   // ← debug 用
    data = other.data;
    tensor_size = other.tensor_size;
    other.data = nullptr;
}
```

跑這段：

```cpp
Tensor make_tensor() {
    Tensor t(1000000);
    return t;                         // 會印 "move called"
}
int main() {
    Tensor result = make_tensor();
    return 0;
}                                     // 會印 "tensor freed"（只一次）
```

預期輸出：
```
move called
tensor freed
```

- `move called` → 證明 `return t` 那行確實呼叫了 move 建構子
- `tensor freed` 只印一次 → 證明資源只被釋放一次（t 已被掏空、解構時被守衛擋住）

> 「加一行 print 確認執行流程」是最基本、最常用的 debug 手法。

---

## 7. 補充：RVO（之後再深入，現在當常識）

現代編譯器有時更聰明，連 move 都省掉，直接讓 `t` 和 `result`
共用同一塊記憶體 → 這叫 **RVO（Return Value Optimization，回傳值優化）**。

- 就算發生 RVO，move 建構子「**有寫**」這件事仍然必要（編譯器要確認它存在）
- 心智模型維持「return 區域變數會 move」即可，這是對的

---

## 速記卡

```
& 接 lvalue（有名字、還要用）
&& 接 rvalue（沒名字、即將消失）

return 區域變數 → 自動變 rvalue → 自動選 move 建構子
std::move(x)    → 手動把 x 標記成 rvalue → 選 move 建構子
                  （std::move 本身不搬東西，只是貼標籤）

move 建構子 = 抄指標 + 對方設 nullptr（過戶，不複製內容）
```