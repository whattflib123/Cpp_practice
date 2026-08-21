# 小考問答紀錄

每個 Stage 開場的 3 題舊觀念小考，以及各 Stage 驗收問題。
之後每次開場小考即時追加。

格式：
- 題目
- **你的答案**（原文）
- ✅ / ❌ + 正解與解說

---

## Stage 8 開場（待補）

---

## Stage 7 開場（2026-08-21）

考 Stage 5、6 舊觀念。

---

**Q1**
```cpp
const std::span<int> s = vec;
s[0] = 999;  // 合法？
```
合法還是編譯錯誤？為什麼？

**你的答案**：合法，因為 const 在這裡修飾 span，不能指向別的地方

✅ 正確。`const` 在 span 物件本身，element type 是 `int`，所以 `s[0]=999` 合法。

---

**Q2**
move ctor 裡為什麼要寫 `other.buf = nullptr`？不寫會怎樣？

**你的答案**：不寫最後會有兩個指標指向同一記憶體，解構時 double-free

✅ 正確。

---

**Q3**
RAII class 的 copy ctor 寫 `= delete` 和「直接不寫」有什麼差別？

**你的答案**：防呆機制，RAII 避免複製，誤用 copy 時會在編譯時卡住

⚠️ 方向對，差一個細節。
「直接不寫」在某些條件下編譯器會自動生成 copy ctor（class 沒有 user-declared move ctor/dtor 時）。`= delete` 是明確禁止，無論任何條件都不會有 copy ctor，且報錯訊息清楚說明「此操作被刪除」。

---

## Stage 7 面試追問（2026-08-21）

---

**Q：`resize` 和 `reserve` 差在哪？**

**你的答案**：不知道 resize；reserve 會預先保留記憶體容量

補充：
- `reserve(n)` 只改 capacity，size 不變，不建構元素
- `resize(n)` 改 size：n > size 補建構元素（預設值），n < size 銷毀尾端元素，可能觸發 reallocation

記法：reserve = 預訂座位（不坐人）；resize = 真的搬人進來或趕出去。

---

**Q：reallocation 時用 move 還是 copy？什麼條件決定？**

**你的答案**：用 copy，由 noexcept 決定，因為編譯器避免搬到一半失敗損毀原始資料

⚠️ 邏輯對，結論說反。
- move ctor 有 `noexcept` → 用 **move**（O(1) per element）
- move ctor 沒有 `noexcept` → 退回 **copy**（O(n)），保證 strong exception safety

---

**Q：哪些 vector 操作會讓 iterator 失效？哪些安全？**

**你的答案**：不知道

正解：
- 失效：`push_back`/`emplace_back`（capacity 不夠時）、`insert`、`erase`、`resize`、`clear`
- 安全：`reserve`（不觸發 reallocation 時）、`operator[]`、`at()`、`front()`、`back()`
- 原則：任何觸發 reallocation 或移動元素位置的操作都讓 iterator 失效

---

## Stage 6 開場（2026-08-20）

考 Stage 4、5 舊觀念。

---

**Q1**
`MallocBuffer` 的 copy ctor 寫了 `= delete`。
面試官問：「不寫 `= delete` 會怎樣？最壞情況是什麼？」

**你的答案**：可能會造成 double free，兩塊指標指向同一個記憶體

✅ 正確。
編譯器預設產生的 copy ctor 是 shallow copy → 兩個物件 `ptr` 指向同一塊記憶體 → 兩個 dtor 都呼叫 `free()` → double-free → UB / crash。
`= delete` 把這個崩潰從執行期提前到編譯期報錯。

---

**Q2**
`Frame` 的 move ctor 沒寫 `noexcept`，但功能上能跑。為什麼面試官還是會說「這是個 bug」？

**你的答案**：因為要加 `noexcept` 才能讓編譯器使用 move 而非 fallback 到 copy；編譯器可能會怕 move 到一半失敗導致資料不完整，而選擇比較安全但比較慢的 copy，喪失 move 的優勢

✅ 正確。
`std::vector` 擴容時需保證 strong exception safety。
- move ctor 有 `noexcept` → 擴容用 move，O(1)
- 沒有 `noexcept` → 退回 copy，O(n)
功能上不崩，但效能悄悄變差，是隱形 bug。

---

**Q3**
這段程式印什麼？為什麼？

```cpp
int arr[8] = {};
int* p = arr;
std::cout << sizeof(arr) << " " << sizeof(p) << '\n';
```

**你的答案**：4 8

❌ 差一步。
正確答案：`32 8`

- `sizeof(arr)` = 32：`arr` 型別是 `int[8]`，`sizeof` **不**觸發 array decay，得到整個陣列大小 8 × 4 = 32
- `sizeof(p)` = 8：`p` 是 `int*`，指標大小，與指向什麼無關（64-bit 系統）
- 你答的 `4` 是 `sizeof(int)`，不是 `sizeof(arr)`

關鍵：傳進函式才 decay；`sizeof` 直接作用在陣列名稱上看到的是原始型別。

```cpp
void f(int* p)       { sizeof(p); }   // 8 — 已 decay，長度消失
void g(int (&a)[8])  { sizeof(a); }   // 32 — reference to array，帶長度
```

這個差異正是 `std::span` 要解決的：裸指標傳入函式長度消失，span 把長度帶著走。

---

## Stage 2 驗收（來源：PROGRESS.md）

考指標與陣列運算觀念。題目與正解從 PROGRESS.md 重建，你的原答已遺失。

**Q1** `buf` 改成 `int*` 後，行為有什麼變化？

正解：步長變 4 bytes（原 `unsigned char*` 是 1 byte），位址不同，byte offset 差 4 倍。

**Q2** `dump` 和 `main` 兩處 `sizeof(buf)` 都印 `8`，為什麼？

正解：array decay — 陣列名稱傳進函式後退化為指標，`sizeof` 只看到指標大小（8 bytes），長度資訊消失。

**Q3** 為什麼把 `buf` 參數宣告為 `const unsigned char*`？

正解：唯讀借用，省複製，編譯期保護原始資料不被改動。

**Q4** `average` 函式為什麼不能把 stride 換成 width？

正解：driver 配置記憶體時行尾有 padding（stride ≥ width），跳下一行必須靠 stride；用 width 跳會算到 padding 位元組，結果錯誤。

---

## Stage 0 驗收（來源：PROGRESS.md）

考工具與 UB 觀念。題目與正解從 PROGRESS.md 重建，你的原答已遺失。

**Q1** `__cplusplus` 印出 `202002L`，`L` 和 `YYYYMM` 格式各是什麼意思？

正解：`L` 是 long literal 後綴；`202002` = 2020 年 02 月，C++20 定案時間。用年月格式是為了單調遞增可做大小比較，`#if __cplusplus >= 202002L` 才能做特性偵測。MSVC 預設回傳 `199711L`，需加 `/Zc:__cplusplus`。

**Q2** `p[3]` 越界但程式「沒崩」，這樣安全嗎？

正解：不安全，是 UB。「沒崩」只是這次的 UB 剛好長得像正常行為。三種真實後果：踩壞 allocator metadata（崩在幾百行後的無辜 `delete`）、踩到相鄰物件、編譯器假設 UB 不發生而反推優化（debug 正常 / release 壞掉）。

**Q3** 為什麼 `int* p` 用 `-Warray-bounds` 抓不到越界，`int a[3]` 可以？

正解：`p` 的型別是 `int*`，長度在型別裡不存在（`new int[3]` 裡的 `3` 用完就消失）；`int a[3]` 的型別是 `int[3]`，編譯器看得到長度。ASan 是執行期工具，看得到真實配置大小。

---

## Stage 5 開場（2026-08-20 補考）

原 transcript 被 compact，題目已遺失；根據當時答案重建題目後重新作答。

---

**Q1**
RAII 的核心概念是什麼？`ctor` 和 `dtor` 各負責什麼？

**你的答案**：ctor 負責創建記憶體的位置，dtor 負責釋放

⚠️ 方向對，精確度差一點。
RAII 的 ctor 是**獲取任何資源**（`malloc`、`fopen`、`pthread_mutex_lock`、`cudaMalloc`…），不只是記憶體。dtor 釋放那個資源；任何離開 scope 的方式都保證 dtor 執行。
面試標準答法：「ctor 獲取資源，dtor 釋放；RAII 保證任何 scope 出口（正常、return、throw）都執行 dtor。」

---

**Q2**
這段程式會 leak 嗎？為什麼？RAII 版本怎麼解決？

```cpp
void process(bool fail) {
    int* p = (int*)malloc(64);
    if (fail) return;
    free(p);
}
```

**你的答案**：可能會，因為 return 發生在 free 前面；RAII 會把 free 等釋放記憶體的方式與 dtor 綁定，確保會執行

✅ 正確。RAII 把釋放綁到 dtor → scope 出去一定跑，無論正常 return 或 throw。

---

**Q3**
下面這行配置了幾個 bytes？

```cpp
void* ptr = malloc(sizeof(64));
```

**你的答案**：4 bytes

✅ 正確。`sizeof(64)` = `sizeof(int)` = 4 bytes。`64` 是 `int` 字面量，`sizeof` 看的是型別大小，不是數值。
配置 64 bytes 要寫 `malloc(64)`，不是 `malloc(sizeof(64))`。

---

## 待補

- Stage 1、3、4 開場小考：transcript 被 compact，問答原文遺失
- Stage 5 Q3 正確題目：需要重建
- Stage 6 之後每次開場小考：往這裡追加
