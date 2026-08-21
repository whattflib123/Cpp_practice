# Stage 進度

最後更新:2026-08-14

## Stage 0 —— 環境確認 ✅ 完成

**目標**:確認 toolchain 可用,且**確認 ASan 真的會抓**(裝了但無效的話,
Stage 2/5 的記憶體練習全白做)。

**已完成**
- WSL2 + Ubuntu 24.04,g++ 13.3.0 確認可用
- `phase0/main.cpp` 寫完:印 `__cplusplus`、`new int[3]`、寫 0~2、
  故意寫 `p[3]` 越界、`delete[]`
  (目錄名沿用 `phase0`,PROGRESS 早期寫的 `stage0/` 已作廢)
- 編譯執行 → ASan 成功報 `heap-buffer-overflow`,
  `WRITE of size 4`,`0 bytes after 12-byte region` ✅
- 已教:ASan 報告四區塊讀法(錯誤類型 / 越界距離 / 兩份 stack trace /
  shadow memory `00 [04] fa`),shadow byte 原理與 ASan vs valgrind 效能差異
- 補 `-g` 重編後,stack trace 出現行號:`main.cpp:16`(寫入點)、
  `main.cpp:6`(配置點)✅
  行號需要兩件事同時成立:編譯期 `-g` 寫入 `.debug_line`,執行期 symbolizer
  把位址反查回原始碼。診斷指令:`readelf -S main | grep debug`
  另裝了 `llvm-18`(提供 `/usr/lib/llvm-18/bin/llvm-symbolizer`),但真正的
  主因是缺 `-g`

**驗收三問(已過)**
- Q1 `202002L` = `YYYYMM`,C++20 定案於 2020/02。用年月是為了單調遞增可比大小,
  才能寫 `#if __cplusplus >= 202002L` 做特性偵測。MSVC 預設回 `199711L`,
  要加 `/Zc:__cplusplus`
- Q2 不安全 —— 是 UB。「沒崩」不是安全的證據,只是這次的 UB 剛好長得像正常行為。
  三種真實後果:踩壞 allocator metadata(崩在幾百行後的無辜 `delete`)、
  踩到相鄰物件、編譯器假設 UB 不發生而反推優化(debug 正常 / release 壞掉)
- Q3 第 16 行 `p` 的型別是 `int*`,長度不在型別裡(第 6 行的「3」用完就消失)。
  對照 `int a[3]; a[3]` 型別是 `int[3]`,`-Warray-bounds` 抓得到。
  → 編譯期只看得到型別裡有的資訊;ASan 是執行期,看得到真實配置大小

**筆記**:`phase0/note.md`(已修正三處:Stage 0 目標是驗證工具而非查版本號、
ASan 非「內建」而是 compiler-instrumented、機制是 redzone + shadow memory
而非「存取前後做邊界檢查」)

## Stage 1 —— 語法暖身 ✅ 完成(目錄 `phase1_warmup/`)

**目標**:手指暖機。重點不是學新語法,是**看到「拷貝真的發生了」**。
考點:range-for by value vs by const&、參數傳遞方式、`auto` 會不會帶走 `&`。

### 已完成:三種 range-for 位址實驗 ✅

實測輸出(`sizeof(Detection)` = 24,元素在 heap 連續排列,間隔 `0x18`):

| 寫法 | 位址 | 改得動本體 | 成本 |
|---|---|---|---|
| `auto d` | 另一塊,五圈全同一格 | ❌ 改到複本 | 每圈一次拷貝 |
| `auto& d` | `&v[i]` | ✅ | 0 |
| `const auto& d` | `&v[i]` | ❌ 編譯期擋 | 0 |

- by value 五圈位址相同 → `d` 圈首建構、圈末解構,下一圈重用同一格
  (Stage 3 物件生命週期的前哨)
- 驗證實驗:`for (auto d : v) d.score = 999;` 後 `v[0].score` 仍是 `0.42`;
  改用 `auto&` 後變 `999`
- `-Wunused-but-set-variable` 只對 by value 那圈發作 —— 編譯器自己指出那是死碼

**結論(慣例)**:預設 `const auto&`;要改元素才 `auto&`;真要複本才 `auto`,
且要註明理由。

**`auto` 推導規則**:丟掉 reference 和 top-level const。`&` 和 `const` 是手寫的,
不是推導來的。

**深拷貝延伸**:`Detection` 是 POD → 拷貝只是 24 bytes memcpy。若加一個
`std::vector<float> mask` 成員,每圈拷貝變成 new + 元素 memcpy + delete。
shallow copy vs deep copy,直通 Stage 4 Rule of Five。

### 已完成:三支函式 ✅

- `print` — `std::fixed + setprecision(2)` 排版，注意 sticky 效果
- `count_above` — `int n = 0` 初始化（未初始化是 UB）
- `filter` — NRVO 優化，回傳 local vector 不會複製

**順帶學到**
- `std::fixed + setprecision(2)` 是 sticky，設完後整個 `std::cout` 都套用
- NRVO 觸發條件：一個函式、一條 return、回傳同一個 local 物件
- NRVO 失效：多條 return path 回不同物件；手動 `return std::move(x)` 反而擋掉

**驗收結論（已過）**
- `auto` 推導丟掉 reference 和 top-level const，`&`/`const` 要手寫
- `const&` 參數 = 唯讀借用：省複製 + 保護原始資料
- 有 heap 資源或 > 64 bytes 才一定傳 `const&`；24 bytes 邊界，含 vector 成員就不行

## Stage 2 —— 指標與陣列運算 ✅ 完成（目錄 `phase2_pointer/`）

**場景**:V4L2 拿到的是裸 `unsigned char*` + `width / height / stride`。
`stride > width`(driver 對齊用的 padding)是 perception pipeline 最常見的入門 bug。

### 已完成 ✅

- `new` 配置 + `std::fill` 整塊填 `0xFF`
- pointer arithmetic 填有效像素：`buf[y * stride + x] = y * 10 + x`
- `dump`：印整塊，padding 位置顯示 `FF`
- `average`：只算有效像素（x < width），正確答案 `17.5`
- stride bug 對比：內圈誤用 stride → `102.5`（被 0xFF 拉高）
- `sizeof` 對比：main/dump 兩處都印 `8`（array decay，指標不帶長度）
- `delete[] buf`

**驗收問題（已過）**
1. `buf` 改 `int*` → 步長變 4，位址不同，byte offset 差 4 倍
2. 兩個 `sizeof` 都印 `8` → array decay；指標不帶長度資訊
3. `const unsigned char*` → 唯讀借用，省複製，保護原始資料
4. 不能省 stride → padding 存在，行尾地址靠 stride 跳

### 待完成

全部完成。

## Stage 3 —— 物件生命週期 ✅ 完成（目錄 `phase3_obj_life/`）

最後更新：2026-08-17

**場景**：frame 傳遞的核心直覺。搞錯傳值方式 = 每幀多複製 8 MB。

### 已完成 ✅

- `Tracker` class：ctor / copy ctor / move ctor / dtor 全帶 log
- 題 3-1：`Tracker a(1)` → 看到 ctor + dtor，LIFO 順序驗證
- 題 3-2：`Tracker b = a` → copy ctor 出現，dtor 兩次
- 題 3-3：`inspect(const Tracker&)` → copy ctor 消失，dtor 少一次
- 題 3-4：NRVO — `return t` 正常編譯無 copy；`-fno-elide-constructors` 關掉後 move 出現
- 題 3-5：`std::move(a)` → move ctor 觸發；moved-from 的 `a` 仍活著（valid but unspecified）

**關鍵觀念（已過）**
- dtor 順序 LIFO：後建構先解構
- `const&` = 唯讀借用，零成本；`&` = 可寫借用；by-value = 複製
- NRVO：一函式一 return 同一 local → 編譯器直接在呼叫端建構，無複製
- move ≠ 自動清空：move ctor 裡要自己把來源資源設 null，防 double free
- moved-from state：valid but unspecified，能 dtor，不能假設值

## Stage 4 —— 拷貝 vs 搬移：Rule of Five ✅ 完成（目錄 `phase4_rule_of_five/`）

最後更新：2026-08-18

**場景**：Frame 持有 heap buffer，copy/move 成本差距在 inference loop 裡才看得見。

### 已完成 ✅

- `Frame` class：`char* buf` + `int size` + `int id`，帶 log 的 ctor/dtor
- 題 4-1：故意不寫 copy ctor → ASan 報 double-free（shallow copy 兩個指標指同一塊）
- 題 4-2：加 copy ctor（deep copy）→ double-free 消失，兩個 dtor 各自清各自的
- 題 4-3：加 copy assignment → `b = a`，先 `delete[] buf` 清舊資源再複製
- 題 4-4：加 move ctor（`noexcept`）→ `other.buf = nullptr` 防 double-free
- 題 4-5：加 move assignment（`noexcept`）→ 先清自己，再接管來源，再清來源

**關鍵觀念（已過）**
- copy ctor vs copy assignment：前者建新物件（無舊資源），後者覆寫已存在（要先 delete）
- move ctor 必須手動 `other.buf = nullptr`：不寫 → dtor 時 double-free
- `noexcept` 對 `std::vector`：有 → 擴容用 move（O(1)）；沒有 → 用 copy（O(n)），原因是 strong exception safety guarantee
- self-assign guard：`if (this == &other) return *this`，在 `delete[] buf` 之前

## Stage 5 —— 把裸 C API 包成 RAII ✅ 完成（目錄 `phase5_raii/`）

最後更新：2026-08-20

**場景**：cudaMalloc / mmap 等 C API 沒有建構子/解構子，中途 return 或 throw 會 leak。

### 已完成 ✅

- 題 5-1：裸 `malloc(64)` + 提早 return → `ASAN_OPTIONS=detect_leaks=1` 確認 leak
  - 坑：`malloc(sizeof(64))` = 4 bytes（sizeof(int)），不是 64
- 題 5-2：`MallocBuffer` RAII class，ctor `malloc`、dtor `free` → leak 消失
- 題 5-3：`return` 換成 `throw` → exception 路徑也觸發 dtor，無 leak
- 題 5-4：`= delete` copy（編譯期擋），加 move ctor（unique ownership）

**關鍵觀念（已過）**
- RAII：任何離開 scope 的方式（正常、return、throw）都保證 dtor 執行
- `= delete`：把 double-free 從執行期崩潰提前變成編譯期報錯
- move-only unique ownership：兩個 owner → 兩個 dtor → double-free
- `unique_ptr` 就是系統寫好的 RAII wrapper，Rule Zero 用它當成員就不用手寫 Rule of Five
- exception safety 三等級：no-throw / strong / basic
- `lock_guard`：RAII wrapper for mutex，dtor 保證解鎖

## Stage 6 —— `std::span` 借用語意 ✅ 完成（目錄 `phase6_span/`）

最後更新：2026-08-20

**場景**：TensorRT output buffer 後處理，裸指標傳入函式長度消失，span 帶著走。

### 已完成 ✅

- 題 6-1：`print_raw(int*, int len)` vs `print_span(span<const int>)` — span 自動推長度
- 題 6-2：從 `vector` 建 span，驗證 `vec.data() == s.data()`，`s[0]=999` 穿透到原始資料
- 題 6-3：`span<const int>` — `s[0]=999` 編譯期報錯（read-only location）
- 題 6-4：dangling span — `make_span()` 回傳指向已銷毀 vector 的 span → ASan `heap-use-after-free`
- 題 6-5：`subspan(i*5, 5)` 取每個 bounding box，印 score

**關鍵觀念（已過）**
- span = 非擁有視圖，帶指標 + 長度，零複製，零 heap 配置
- `span<const int>` vs `const span<int>`：前者管元素，後者管 span 物件本身（類比 `const int*` vs `int* const`）
- dangling span：span 生命週期不能超過來源資料，編譯器不擋，只有 ASan 抓
- `span<const int>` 函式參數比 `const vector<int>&` 更通用（接受 array、vector、raw pointer 全部）
- `string_view` 是字串版 span，概念相同，多字串 API

## Stage 7 —— `std::vector` capacity vs size ✅ 完成（目錄 `phase7_vector_capacity/`）

最後更新：2026-08-21

**場景**：detection queue 效能，push_back 為什麼有時候慢。

### 已完成 ✅

- 題 7-1：觀察 capacity 翻倍成長（×2 規律）
- 題 7-2：`reserve(20)` 消除所有 reallocation
- 題 7-3：`push_back` 觸發 reallocation → iterator 失效 → ASan `heap-use-after-free`
- 題 7-4：`reserve` 夠大 → 無 reallocation → iterator 安全

**關鍵觀念（已過）**
- `size` = 元素數；`capacity` = 配置空間；reallocation 在 size==capacity 時觸發
- amortized O(1)：capacity ×2 策略，n 次 push_back 總搬移量 2n，平均 O(1)
- `reserve` 預訂容量不建構元素；`resize` 真的建構（或銷毀）元素
- reallocation 用 move 還是 copy：move ctor 有 `noexcept` → move；沒有 → copy（strong exception safety）
- iterator invalidation：任何觸發 reallocation 或移動元素的操作都讓 iterator 失效

## Stage 8 —— 未開始

大綱見 repo root 的 `CLAUDE.md`。

## 已順帶教過的東西(避免重複)

- `collect2: error: ld returned 1 exit status` = **連結期**錯誤,不是語法錯。
  編譯期 vs 連結期的分界(面試題:「undefined reference to X 怎麼查」)
- `'\n'` vs `std::endl` —— endl 多一次 flush,迴圈裡印 log 是效能地雷
- `__cplusplus` 是 preprocessor 預定義巨集,值由 `-std=` 決定,反映**編譯當下**
  的旗標而非執行環境
- Linux 執行本地檔要 `./main`,`.` 不在預設 PATH
- `apt install` 出現 `404 Not Found` = 本機 package index 過期,指向已被取代的
  檔名。先 `sudo apt update` 再裝
- 裝工具挑最小的包:`llvm-18` 而非 meta package `llvm`(後者多拖 700MB dev 檔)
