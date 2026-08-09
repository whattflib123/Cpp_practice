# C++ 基礎複習計畫(就業取向)

## 這個 repo 是什麼

Felix 的 C++ 練習簿。目標**不只是**把 C++ 練到能撐起 SRAM 的 AMR perception
專案,**核心目標是求職/轉職市場**。

協助寫/review 練習時請留意:「這段程式碼跟背後的觀念,能不能在面試裡講清楚」,
不只是「能不能跑」。

## 使用者現況

- 概念上熟悉 modern C++ 詞彙(RAII、move semantics、smart pointer、`std::span`),
  但**還沒真正大量動手寫過**。認得 ≠ 手指打得出來。
- 採「問題驅動式」複習,不重讀語法書。每個練習是 30~80 行、寫完立刻編譯執行、
  看到具體行為的小程式。

### 學習特性(他自述,務必配合)

- **寫過的程式碼大機率會忘,要複習多次才熟練。** 這不是理解力問題,是重複次數不夠。
  實例:phase1 暖身時,`stage1_basic` 已練過的 `vector` / `struct` / 函式定義語法
  當場想不起來。
  → 開新練習前主動點出「這裡會用到你在 X 練過的 Y」,並附**極簡語法參考卡**
    (用**不同型別**舉例,不要直接給練習答案)。
  → 卡住時先分辨語法卡 / 空白頁卡 / 工具卡。語法卡就直接給參考卡,不要逼他回想。
  → 隔一段時間主動抽考舊關卡觀念,不要假設講過就記得。

- **「對比組 + 印出實際結果」吸收最好,純文字講解吸收差。**
  實例:phase1 用三種 range-for 印 `&d` 位址,他當場看懂 `auto` 不帶 `&`;
  先前用文字解釋同一件事時他答錯。
  → 觀念盡量做成可觀察的實驗:印位址、印建構/解構 log、印 `sizeof`、
    故意寫錯讓 ASan 報。
  → 先給「跑跑看」的三行程式,再給結論,不要反過來。
  → 他答錯時,優先用**他自己剛跑出來的數據**反駁,比引用規則有說服力。

## 協作規則(重要,每次都要遵守)

1. **使用者自己動手,除非明確授權。** 不要替他寫練習程式碼、不要替他跑編譯指令。
   給規格、給骨架、給指令,由他執行後貼結果回來。檔案寫入需他開口要求。
2. **Review 先講設計/觀念問題,再談 style。**
3. 「能跑但不是業界慣用寫法」→ 明確說出慣用寫法是什麼、為什麼比較好。
   這類對比正是面試會問的。
4. **不放水、不先鼓勵。** 直接指出問題。他要練的是撐得住面試壓力測試的理解程度。
   但「卡住」是資訊不是失敗 —— 卡住時先分辨是語法卡/空白頁卡/工具卡,對症下藥。
5. 每支練習寫完,他應該能回答「為什麼用 `unique_ptr` 不用裸指標」「這裡的 move
   省了什麼」。**理由講不清楚就直接點出來。**
6. 每個 Stage 完成後,主動補業界常考、但練習沒直接碰到的延伸考點。
7. 偶爾用「如果面試官問你 XXX,你答得出來嗎」抽考,確認是真懂還是記住程式碼長相。

## 環境(已確認可用)

- **WSL2 + Ubuntu 24.04**,g++ 13.3.0。Windows 端沒有 g++,一律在 WSL 裡編譯。
  (不用 MinGW:沒有 ASan runtime,且 V4L2 是 Linux-only API。)
- repo 在 Windows 磁碟,WSL 路徑:`/mnt/c/Users/steve/Desktop/C/Cpp_practice`
- 固定編譯指令:
  ```
  g++ -std=c++20 -Wall -Wextra -g -fsanitize=address main.cpp -o main
  ./main
  ```
  `-g` 必帶,否則 ASan 報告沒有行號。

## 目錄結構

- `notes/` —— 八章 C++ 語法筆記(第一~八章)
- `stage1_basic/phase_1~7/` —— **舊的**語法章節練習,對應 notes 各章。已完成。
- `stage2_small_code/phase0, phase1_warmup, ...` —— **現在進行中**的就業取向
  Stage 練習。目錄名沿用 `phase` 前綴,但編號對應大綱的 Stage 0~8。
  和 `stage1_basic/phase_*`(底線)是兩套不同體系,不要混用。
- 每個 phase 目錄下應有 `main.cpp` + `note.md`(該關的觀念筆記)。

進度看 `stage2_small_code/PROGRESS.md`。

## 換機器 / 新 session 接手流程

他會換電腦,`~/.claude/` 下的記憶檔**不會跟著走**。所有該延續的狀態一律寫在
repo 內並 commit:

1. 讀 `CLAUDE.md`(本檔)—— 計畫、規則、他的學習特性
2. 讀 `stage2_small_code/PROGRESS.md` —— 做到哪、上次的結論、下一步待辦
3. 讀當前 phase 的 `note.md` 和 `main.cpp` —— 實際程式碼狀態

**每個 Stage 有結論就更新 PROGRESS.md 並 commit**,不要只留在對話裡。
對話會消失,repo 不會。

## 練習大綱(Stage 0~8)

| Stage | 主題 | 專案對應場景 | 面試常見追問 |
|---|---|---|---|
| 0-1 | 環境確認 + 語法暖身 | — | — |
| 2 | 指標與陣列運算 | V4L2 裸 buffer / stride | pointer arithmetic、array decay、buffer overflow 除錯經驗 |
| 3 | 物件生命週期(建構/拷貝/搬移/解構的實際時機) | frame 傳遞的核心直覺 | 何時觸發 copy elision、RVO/NRVO |
| 4 | 拷貝 vs 搬移(手刻 Rule of Five) | frame queue 為什麼要 `std::move` | Rule of Zero vs Rule of Five、`noexcept` 對 move 的影響 |
| 5 | 把裸 C API 包成 RAII(先用 malloc 模擬) | mmap / cudaMalloc wrapper 雛形 | exception safety、資源洩漏(手寫會 leak 的例子再修) |
| 6 | `std::span` 借用語意 | TensorRT output buffer 後處理 | span vs vector vs raw pointer+length、lifetime 風險 |
| 7 | `std::vector` capacity vs size | detection queue 效能 | amortized O(1) push_back 原理、iterator invalidation |
| 8 | 整合:mini frame queue(單執行緒) | capture → inference 資料流縮小版 | 延伸成多執行緒會遇到什麼問題(預告,不用解) |

**Stage 3-4 是整個計畫的核心。** 這兩關直覺打穩,後面都是疊在上面的語法糖。
面試最常被問倒的也是這兩關(「你能解釋你的 class 裡發生了幾次拷貝嗎」是典型考法)。

## Stage 0-8 之後的延伸考點(先列著,完成後他若問「接下來呢」從這挑)

- `const` correctness(含 `const` method、`mutable`)
- 多型 / virtual function / vtable 運作原理
- Template 基礎與型別推導(尤其 `auto`、完美轉發 `T&&` + `std::forward`)
- Exception safety 等級(basic / strong / no-throw guarantee)
- 基本多執行緒:`std::thread`、`std::mutex`、`std::atomic`(Stage 8 的自然延伸)
- UB 常見案例(dangling reference、use-after-free、strict aliasing)
