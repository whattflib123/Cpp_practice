# Stage 5 — 把裸 C API 包成 RAII

## 為什麼這關重要

Stage 4 的 `Frame` 自己管 `new/delete`（C++ 風格）。
現實中你常遇到的是**別人的 C API**：

| C API | 配置 | 釋放 |
|---|---|---|
| 標準 C | `malloc` | `free` |
| Linux 記憶體映射 | `mmap` | `munmap` |
| CUDA GPU 記憶體 | `cudaMalloc` | `cudaFree` |
| File descriptor | `open` | `close` |

這些 API 沒有建構子/解構子，資源管理全靠你手動呼叫。
一旦中途 `return` 或丟出例外，`free`/`close` 就被跳過 → **resource leak**。

RAII 的解法：把「呼叫 `free()`」的責任丟給 dtor。
物件活著 = 資源活著；物件死亡 = 資源自動釋放。任何離開 scope 的方式都觸發 dtor，包括 exception。

---

## RAII 是什麼

**Resource Acquisition Is Initialization**

- **建構子**：向 C API 要資源（`malloc`/`open`/`cudaMalloc`）
- **解構子**：還回去（`free`/`close`/`cudaFree`）
- 中間的 exception 或 early return？C++ 保證 dtor 一定被呼叫。

```
{
    RaiiBuffer buf(1024);   // malloc in ctor
    if (error) return;      // ← dtor 自動呼叫，free 不會漏
    // ...
}                           // ← 正常離開，dtor 也呼叫
```

---

## 語法參考卡（用 `FileHandle` 舉例，不是練習答案）

```cpp
class FileHandle {
    int fd;
public:
    explicit FileHandle(const char* path)
        : fd(open(path, O_RDONLY)) {}

    ~FileHandle() {
        if (fd >= 0) close(fd);
    }

    // RAII wrapper 通常是 move-only（unique ownership）
    FileHandle(const FileHandle&)            = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept : fd(other.fd) {
        other.fd = -1;   // sentinel：-1 = 已轉走
    }
    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this == &other) return *this;
        if (fd >= 0) close(fd);
        fd = other.fd;
        other.fd = -1;
        return *this;
    }
};
```

`= delete`：告訴編譯器「這個函式不存在」。
呼叫端嘗試 copy → 直接編譯錯誤，比執行期崩更好。

---

## 微題階梯

### 題 5-1：裸 malloc/free → 觀察 leak

寫一個函式 `process(bool fail)`：
- `malloc(64)` 一塊
- 如果 `fail == true`，提早 `return`（沒有 `free`）
- 正常路徑 `free`

```cpp
process(true);   // 提早 return，buf 沒 free → leak
process(false);  // 正常 free
```

用 `ASAN_OPTIONS=detect_leaks=1 ./main` 執行，觀察 ASan 報 leak。

---

### 題 5-2：包成 RAII class

寫 `MallocBuffer` class：
- 成員：`void* ptr`、`size_t size`
- ctor：`ptr = malloc(size)`，印 `"malloc <size> bytes"`
- dtor：`free(ptr)`，印 `"free   <size> bytes"`

把題 5-1 的 `process` 改用 `MallocBuffer`，再用 `ASAN_OPTIONS=detect_leaks=1` 跑，確認 leak 消失。

---

### 題 5-3：exception safety

把「提早 return」換成「丟例外」：

```cpp
void process(bool fail) {
    MallocBuffer buf(64);
    if (fail) throw std::runtime_error("oops");
    // ...
}

try { process(true); }
catch (...) { std::cout << "caught\n"; }
```

預期：即使 `throw`，`buf` 的 dtor 仍被呼叫 → `free` 執行，不 leak。

---

### 題 5-4：move-only（刪除 copy）

RAII wrapper 通常只允許 move（一份資源只能有一個 owner）：

```cpp
MallocBuffer(const MallocBuffer&)            = delete;
MallocBuffer& operator=(const MallocBuffer&) = delete;
```

加完後，故意寫 `MallocBuffer b = a;`，確認編譯期就報錯（不是執行期崩）。
再加 move ctor，讓 `MallocBuffer b = std::move(a)` 可以編過。

---

## 關鍵觀念（練完應能回答）

1. **RAII 保證了什麼？** — 物件離開 scope（任何方式，含 exception）dtor 一定執行
2. **為什麼 RAII wrapper 通常 delete copy？** — 資源只能有一個 owner；兩個 wrapper 指同一塊 → double-free
3. **`= delete` 和「不寫」有什麼差？** — 不寫時編譯器可能自動生成；`= delete` 明確禁止，呼叫端編譯期就報錯
4. **`explicit` 關鍵字是什麼？** — 禁止隱式轉換：`MallocBuffer buf = 64;` 這種寫法會被擋掉

---

## 面試常考追問

- 「RAII 和 smart pointer 有什麼關係？」（`unique_ptr` 就是 RAII wrapper for pointer）
- 「exception safety 有幾個等級？」（basic / strong / no-throw）
- 「`std::lock_guard` 為什麼是 RAII？」（ctor 上鎖，dtor 解鎖，throw 也不會死鎖）
- 「Rule of Zero 是什麼？」（讓成員自己是 RAII type，自己的 class 不手寫任何 special member）
