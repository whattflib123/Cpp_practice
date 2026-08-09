# Stage 進度

最後更新:2026-08-09

## Stage 0 —— 環境確認(進行中,快收尾)

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

**待辦**
1. **未回答的三個驗收問題**:
   - Q1 `__cplusplus` 印出什麼數字?為什麼不是 `20` 或 `2020`?
   - Q2 假設 ASan 沒報錯、程式安靜跑完,代表寫 `p[3]` 是安全的嗎?
     (考點:UB vs 「看起來沒事」)
   - Q3 `-Wall -Wextra` 為什麼沒抓到?(考點:編譯期 vs 執行期的界線;
     `p` 是 `int*`,編譯器手上還有 `3` 這個資訊嗎?)

   **Q2、Q3 答得好不好,決定能不能進 Stage 2。**

## Stage 1~8 —— 未開始

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
