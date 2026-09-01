# 小考問答紀錄

每個 Stage 開場的 3 題舊觀念小考，以及各 Stage 驗收問題。
之後每次開場小考即時追加。

格式：
- 題目
- **你的答案**（原文）
- ✅ / ❌ + 正解與解說

---

## Stage 9-5 前複習（2026-08-25）

---

**Q1：`cv.wait` 的 predicate lambda 在防什麼？拿掉會怎樣？**

**你的答案**（第一次）：都不會
**你的答案**（第二次）：是在防止 Frame 裡面是空的，拿到錯誤的資料

❌ 漏掉關鍵詞。
防的是 **spurious wakeup**：OS 偶爾無緣無故喚醒 `wait`（沒人 `notify`）。沒有 predicate，醒來就繼續跑，frames 還是空 → pop 空 vector → crash。predicate 等價於 `while (!pred()) cv.wait(lk)`，spurious wakeup 後重新檢查，不滿足就繼續睡。

---

**Q2：`set_done()` 為什麼要放在 producer thread 裡最後呼叫，不能放在 main thread？**

**你的答案**（第一次）：都不會
**你的答案**（第二次）：因為會喚醒所有 consumer 嗎

❌ 和 `notify_all()` 混淆了。
正確原因：main thread 和 producer thread 同時跑。從 main thread 呼叫 `set_done()` 可能在 producer push 任何 frame 之前就執行——consumer 看到 `done=true && empty=true` 立刻退出，0 個 frame 被處理。放進 producer lambda 最後一行，保證 5 個全 push 完才設。

---

**Q3：這個 queue 的效能瓶頸在哪？**

**你的答案**（第一次）：都不會
**你的答案**（第二次）：要等到所有 producer 都做完才會進到 consumer

❌ 搞混了單執行緒和多執行緒。producer 和 consumer 是同時跑的（兩個 thread），不是序列。
真正的瓶頸：**單一 mutex 鎖住整個 vector**，push 和 pop 不能同時進行。改善方向：lock-free queue 或 double-buffer（一個給 push、一個給 pop，交換時才鎖）。

---

## Stage 10-3 前複習（2026-08-28）

---

**Q1：Rule of Five 五個是哪五個？**

**你的答案**：dtor、copy ctor、copy assignment、move ctor、move assignment

✅ 正確。

---

**Q2：`DmaBuffer` 為什麼要 `= delete` copy constructor？**

**你的答案**：為了確保 runner 會使用 move 而不是 copy

⚠️ 部分對，但根本原因沒說到。
根本原因：**防止 double free**。若允許 copy，兩個物件的 `ptr` 指向同一塊記憶體，各自 destructor 都呼叫 `free()` → double free → crash。「逼呼叫方用 move」只是副作用，不是原因。

---

## Stage 9 開場（2026-08-21）

考 Stage 4、5、8 舊觀念。

---

**Q1**
`Frame` 用 `std::unique_ptr<char[]>` 當成員，沒有寫 dtor。為什麼不會 leak？

**你的答案**：因為已經透過 unique_ptr 管理物件的生命週期了

✅ 正確。`unique_ptr` dtor 自動 `delete[]`，scope 結束保證執行，Rule of Zero。

---

**Q2**
`lock_guard` 是什麼？為什麼用它而不是手動 `mutex.lock()` / `mutex.unlock()`？

**你的答案**：忘記 mutex 了

❌ 需要複習。
`lock_guard` 是 mutex 的 RAII wrapper：ctor `lock()`，dtor `unlock()`。
手動 lock/unlock 若中間 throw 或 return，unlock 永遠不執行 → deadlock。
`lock_guard` 任何 scope 出口都保證 unlock，與 `MallocBuffer` 包 `malloc/free` 同一概念。

---

**Q3**
`noexcept` 沒寫在 move ctor 上，`std::vector` 擴容時會發生什麼？為什麼？

**你的答案**：vector 擴容時可能怕資料搬遷失敗造成資料毀損，fallback 選用 copy，導致比 move 慢

✅ 完整。沒有 `noexcept` → 退回 copy（O(n)）保 strong exception safety；有 `noexcept` → 用 move（O(1)）。

---

## Stage 8 面試追問（2026-08-21）

---

**Q：為什麼用 `unique_ptr` 而不是手寫 Rule of Five？**

**你的答案**：省去管理麻煩

⚠️ 方向對，面試要說完整：
`unique_ptr` 已是 move-only RAII wrapper，成員用它後 compiler 自動產生的 copy/move/dtor 全部正確（copy 被 delete、move 轉移所有權、dtor 自動 delete[]）。不需要手寫任何 special member，也不會有寫錯的風險。這就是 **Rule of Zero**。

---

**Q：`push(Frame&& f)` 為什麼是 `&&`？傳 `Frame f` by value 有什麼差？**

**你的答案**：省去建構的麻煩；後補：`&&` 明確讓呼叫者只能傳 rvalue，強制用 `std::move`

✅ 第二次補充答到核心：`&&` 強制呼叫方交出所有權，編譯期擋住誤傳 lvalue。

---

**Q：`frames.reserve(16)` 不寫會怎樣？**

**你的答案**：可能不斷 reallocate 會變慢

⚠️ 對，但少一個更嚴重的點：reallocation 讓所有 iterator 和指標失效（dangling）。`reserve` 同時解決效能和安全兩個問題。

---

## Stage 8 開場（2026-08-21）

考 Stage 4、6、7 舊觀念。

---

**Q1**
什麼條件下 `push_back` 之後舊的 iterator 還是有效的？

**你的答案**：沒有超過 capacity 大小，不需要 reallocate 的情況

✅ 正確。capacity 夠大不觸發 reallocation → 舊 iterator 有效。

---

**Q2**
寫一個回傳 `std::span<int>` 的函式，什麼情況下安全、什麼情況下 dangling？

**你的答案**：把 span 和 RAII 的 return 問題搞混了，忘記 span 是什麼

❌ 需要複習。
- dangling：函式內建 local vector，回傳指向它的 span → vector 死了，span 指向釋放的記憶體
- 安全：span 指向 caller 傳入的資料，或靜態/全域資料，caller 活著 span 就有效
- 關鍵：span 生命週期不能超過它指向的資料

---

**Q3**
`Frame` 有 `char* buf`，只寫了 dtor，沒寫 copy ctor。`Frame b = a` 後兩個 dtor 各自執行，會發生什麼？

**你的答案**：會有 double delete，因為兩個指標指向同一個記憶體

✅ 正確。compiler 預設 copy ctor 是 shallow copy → 兩個 buf 指向同一塊 → 兩個 dtor delete[] → double-free。

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

## Stage 10 開場（2026-08-31）

---

**Q：`std::span<float>` 和 `const std::vector<float>&` 差在哪？**

**你的答案**：span 只把指標資訊帶進去，可以修改；後者不行

⚠️ 方向對，重點說錯了。
- `std::span<float>` 預設**可修改**；`const std::vector<float>&` 不可修改——你說反了，但這不是重點。
- 關鍵差異：
  1. **non-owning**：span 只是 `(ptr, size)`，不複製、不 malloc；vector& 是真正的 vector 有自己記憶體
  2. **接受任何連續記憶體**：span 能接 raw array、vector、裸指標+長度；`vector&` 只能接 vector
- perception pipeline 為什麼重要：DmaBuffer 是裸 `void*`，不是 vector。傳 `vector&` 就要多一次 copy，span 是 zero-copy borrow。

---

## Stage 10 驗收（2026-08-31）

---

**Q1：`Frame` 為什麼用 `unique_ptr` 管 buffer？**

**你的答案**：就不用手動管理物件資源

✅ 正確。完整說法：`unique_ptr` dtor 自動 `delete[]`，離開 scope 保證釋放（RAII），不會 leak，也不用手寫 destructor。

---

**Q2：`FrameQueue::pop()` 為什麼用 `unique_lock` 不用 `lock_guard`？**

**你的答案**：因為會需要透過條件等改變鎖的狀態

✅ 正確。`cv.wait()` 內部需要暫時 unlock（讓 producer 能進臨界區 push），再重新 lock。`lock_guard` 無法手動操作鎖狀態，`unique_lock` 可以。

---

**Q3：`InferenceEngine::run()` 為什麼收 `span<float>` 不收 `vector<float>&`？**

**你的答案**：因為要直接去用到資料本身嗎

❌ 太模糊。
正確答案：**zero-copy borrow**。`span` 只是 `(ptr, size)`，不複製資料。如果收 `vector<float>&`，呼叫方必須把 `unique_ptr<char[]>` 的資料 copy 進一個 vector——多一次 malloc + memcpy，在 perception pipeline 是效能殺手。另外 `span` 接受任何連續記憶體（裸指標、array、vector），`vector&` 只能接 vector。

---

## Stage 12（多型）開場複習（2026-08-31）

---

**Q1：`cv.wait()` 為什麼不能搭配 `lock_guard`，要用 `unique_lock`？**

**你的答案**：因為只有 lock_guard 可以受到條件解鎖

❌ 搞反了。是 `unique_lock` 才能條件解鎖，不是 `lock_guard`。
`cv.wait()` 內部：解鎖 → 睡眠 → 被喚醒 → 重新上鎖。需要 `unlock()` method。
`lock_guard` 沒有 `unlock()`，只有 dtor 才解鎖，所以 `cv.wait()` 無法接受它。

---

**Q2：move ctor 裡為什麼一定要寫 `other.buf = nullptr`？不寫會發生什麼？**

**你的答案**：不然可能會出現兩個指標指向同一個位址，會發生 double free

✅ 正確。move 後兩個物件都有 dtor，不清 null → 兩個 dtor 都 `delete` 同一塊 heap → double free。

---

**Q3：`span<const float> data` vs `const span<float> data`，各自能做什麼、不能做什麼？**

**你的答案**：A 不能改 data，B 不能改 span 的指向

⚠️ 答對了限制，沒答另一半。
- A（`span<const float>`）：不能改元素，**可以** reseat span 本身。類比 `const float*`。
- B（`const span<float>`）：不能 reseat，**可以** 改元素（`data[0] = 1.0f` 合法）。類比 `float* const`。
實務上 A 更常用：借用 buffer 只讀時用 `span<const T>`。

---

## Stage 13（Template）開場複習（2026-09-01）

---

**Q1：`unique_ptr` 為什麼不能 copy，只能 move？**

**你的答案**：因為 unique_ptr 要確保一個物件只能由一個指標管理，避免 double free

✅ 正確。unique ownership → copy 會產生兩個 owner → 兩個 dtor → double free，所以 copy 被 `= delete`，只能 move 轉移所有權。

---

**Q2：`span<const float>` 和 `const span<float>` 差在哪？**

**你的答案**：前者可以改 span 的指向，不能改指向的值；後者可以改值但不可以改指向

✅ 正確。`span<const float>` = 元素唯讀，span 本身可 reseat；`const span<float>` = 元素可寫，span 不可 reseat。類比 `const float*` vs `float* const`。

---

**Q3：base class 有 `virtual` function，dtor 一定要加什麼？為什麼？**

**你的答案**：= default，要確保每個建立起來的物件都會被解構

⚠️ 語法沒錯但關鍵字答錯，原因也不完整。
正確答案：dtor 要加 **`virtual`**（`virtual ~Base() = default;`）。
原因：`delete base_ptr` 時，沒有 `virtual` → 靜態型別決定呼叫哪個 dtor → derived dtor 沒跑 → leak / UB。
`= default` 只是「用編譯器產生的實作」，不加 `virtual` 一樣會出問題。

---

## Stage 13（Template）練習中問答（2026-09-01）

---

**Q：三次呼叫都沒有寫 `my_max<int>(...)`，編譯器怎麼知道 `T` 是什麼？**

**你的答案**：透過 template 編譯器會將輸入的型別一同傳入 function

⚠️ 方向對，說法不精確。
正確說法：**template argument deduction**——編譯器看引數型別自動推導 `T`。`my_max(3, 5)` 兩個引數都是 `int` → `T = int`。陷阱：`my_max(3, 1.5)` 一個 `int` 一個 `double`，`T` 衝突 → 編譯期報錯，不自動轉型。

---

**Q：`const` 和 `&` 在 template 推導後還在嗎？這和 `auto` 有什麼關係？**

**你的答案**：不知道

❌ 未答。
正確答案：`const` 和 `&` 都丟掉，推導結果都是 `int`。`auto` 推導規則和 template 完全相同——都丟掉 reference 和 top-level const。要保留要手寫：`const T&` / `auto&`。

---

**Q：`foo(int&&)` 和 `bar(T&&)` 都寫 `&&`，為什麼 `bar(a)` 可以接 lvalue，`foo(a)` 不行？**

**你的答案**：因為有用 typename，foo(a) 無法連結到 lvalue

⚠️ 方向對，說法不精確。
正確說法：`foo(int&& x)`：`int` 固定，`&&` 是 rvalue reference，只接 rvalue。`bar(T&& x)`：`T` 是 template 參數，`T&&` 是 **forwarding reference**，推導規則特殊——傳 lvalue `a` 時 `T` 推導成 `int&`，`int& &&` 折疊成 `int&`（reference collapsing）。

---

**Q：`wrapper_bad(42)` 印出的是 lvalue 還是 rvalue？為什麼？**

**你的答案**：印出的是 lvalue，因為沒有把型別傳進去

⚠️ 結果答對，原因不精確。
正確原因：有名字的變數就是 lvalue。`x` 進了函式就有名字，不管外面傳的是 rvalue `42`，函式體內 `x` 是 lvalue。`std::forward` 的工作是根據 `T` 推導結果把原本的值類別還原。

---

**Q：`make_obj` 裡的 `Args&&...` 是什麼？`std::forward<Args>(args)...` 在做什麼？**

**你的答案**：都不知道

❌ 未答。
正確答案：`Args&&...` 是 **variadic template**，`...` 表示零個或多個型別參數，`Args` 是參數包。`std::forward<Args>(args)...` 展開參數包，同時把每個參數的值類別還原，等同 `forward<int>(1), forward<const char*>("cam0")`。這是 `std::make_unique` 內部的實作原理。

---

## 待補

- Stage 1、3、4 開場小考：transcript 被 compact，問答原文遺失
- Stage 5 Q3 正確題目：需要重建
- Stage 6 之後每次開場小考：往這裡追加
