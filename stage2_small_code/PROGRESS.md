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

## Stage 3~8 —— 未開始

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
