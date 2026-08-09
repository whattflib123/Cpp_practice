# Stage 進度

最後更新:2026-08-09

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

## Stage 1 —— 語法暖身(進行中,目錄 `phase1_warmup/`)

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

### 待辦:三支函式

`print` / `count_above` / `filter` 只有宣告沒有定義(未被呼叫,所以連結器不報錯
—— 順帶學到「宣告 vs 定義」與連結期只解析用到的符號)。

兩支函式是**刻意的對照組**,差別在回傳型別:
- `count_above` 回傳 `int` —— 便宜的基準線
- `filter` 回傳 `std::vector<Detection>` —— 考「回傳大物件貴不貴」。
  答案是不貴,靠 NRVO,退而求其次靠 move。C++98 年代要用 `out` 參數規避。
  **真正的證明留到 Stage 3 加建構/解構 log 時揭曉**

繼續前記得把 999 那段驗證程式碼刪掉,否則 `count_above` 結果會亂。

**原始規格**:`phase1/main.cpp`
1. `struct Detection { int id; float score; float x, y, w, h; };`
2. `std::vector<Detection>` 塞 5 筆(score 有高有低)
3. 三種迴圈各跑一次,每圈印元素位址 `&d`:
   `for (auto d : v)` / `for (const auto& d : v)` / `for (auto& d : v)`
   對照 `&v[0]` —— by value 那圈位址會不同(是複本)
4. `int count_above(const std::vector<Detection>& v, float thr)` 計 score 超過門檻的筆數
5. `std::vector<Detection> filter(const std::vector<Detection>& v, float thr)`
   回傳新 vector
6. `void print(const Detection& d)`,用 `std::fixed` + `setprecision(2)` 排版

**驗收問題**
1. 三種迴圈位址差在哪?`auto` 為什麼不會自動帶走 `&`?
2. `filter` 回傳整個 vector,會不會複製一份?為什麼?(RVO / move)
3. `const std::vector<Detection>&` 的 `const` 和 `&` 各擋掉什麼?
4. `Detection` 24 bytes,by value 傳有什麼問題?多大才該改傳 reference?

## Stage 2 —— 指標與陣列運算(未開始)

**場景**:V4L2 拿到的是裸 `unsigned char*` + `width / height / stride`。
`stride > width`(driver 對齊用的 padding)是 perception pipeline 最常見的入門 bug。

**規格**:`phase2/main.cpp`,`width=6 / height=4 / stride=8`
1. `new unsigned char[stride * height]`,整塊先填 `0xFF` 當 padding 哨兵
2. 只用 pointer arithmetic(禁二維陣列語法)填有效像素:
   第 y 行起點 `buf + y * stride`,只填前 `width` 格,值 `y * 10 + x`
3. `void dump(const unsigned char*, int width, int height, int stride)` 印整塊含 padding
4. `double average(const unsigned char*, int width, int height, int stride)`
   只計有效像素;若誤用 stride 當內圈上限,平均會被 255 拉高
5. 函式內外各印一次 `sizeof(buf)`,比較(考點:array decay)
6. 故意存取第 `height` 行看 ASan 報什麼,再註解掉重跑
7. `delete[] buf`

**驗收問題**
1. `buf + y * stride + x`,若 `buf` 改成 `int*` 算出的位址一樣嗎?為什麼?
2. 兩個 `sizeof` 印出什麼?為什麼不同?這現象叫什麼?
3. `average` 的參數為什麼是 `const unsigned char*`?
4. 為什麼不能省略 `stride` 直接用 `width` 算位址?

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
