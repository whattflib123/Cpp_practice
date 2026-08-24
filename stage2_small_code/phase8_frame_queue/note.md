# Stage 8 — Mini Frame Queue（整合）

## 這關學什麼

把 Stage 3~7 全部串起來，做一個單執行緒的 frame queue：

```
capture → push Frame into queue → pop Frame → inference（用 span 讀資料）
```

每個 Frame 持有 heap buffer（Stage 4/5），queue 用 vector（Stage 7），
傳遞用 move（Stage 4），讀資料用 span（Stage 6）。

---

## 語法參考卡

```cpp
// Frame — RAII + move-only（Stage 4/5）
class Frame {
    std::unique_ptr<char[]> buf;
    int id;
    int size;
public:
    Frame(int id, int size);
    // Rule of Zero：unique_ptr 自動管理，不需手寫 dtor/copy/move
};

// FrameQueue — vector + reserve（Stage 7）
class FrameQueue {
    std::vector<Frame> frames;
public:
    void push(Frame&& f);           // move in
    Frame pop();                    // move out
    bool empty() const;
};

// 讀資料 — span（Stage 6）
void process(std::span<const char> data);
```

---

## 微題階梯

### 題 8-1：`Frame` class（Rule of Zero 版）

用 `std::unique_ptr<char[]>` 當 buf，不手寫任何 special member function。

```cpp
class Frame {
    std::unique_ptr<char[]> buf;
    int id;
    int size;
public:
    Frame(int id, int size)
        : buf(std::make_unique<char[]>(size)), id(id), size(size) {
        std::cout << "alloc #" << id << '\n';
    }
    // 不寫 dtor / copy / move — unique_ptr 處理一切
    int get_id()   const { return id; }
    int get_size() const { return size; }
    const char* data() const { return buf.get(); }
};
```

在 main 建兩個 Frame，用 `std::move` 把一個移給另一個，印 id 確認 move 後來源失效。

---

### 題 8-2：`FrameQueue` — push / pop

```cpp
class FrameQueue {
    std::vector<Frame> frames;
public:
    FrameQueue() { frames.reserve(16); }  // 預防 reallocation

    void push(Frame&& f) {
        frames.push_back(std::move(f));
    }

    Frame pop() {
        // TODO: 取出最後一個（back），move 出來，pop_back
    }

    bool empty() const { return frames.empty(); }
    size_t size() const { return frames.size(); }
};
```

完成 `pop()`，在 main push 三個 frame、pop 一個，確認 id 正確。

---

### 題 8-3：`process` — 用 `span` 讀 Frame 資料

```cpp
void process(std::span<const char> data) {
    std::cout << "processing " << data.size() << " bytes\n";
    // 印前 4 bytes 的數值（假資料）
}
```

在 main 用 `std::span<const char>(f.data(), f.get_size())` 把 Frame 資料傳進去。

---

### 題 8-4：完整 pipeline

```cpp
int main() {
    FrameQueue q;

    // capture：push 3 個 frame
    for (int i = 0; i < 3; i++) {
        Frame f(i, 64);
        // 填假資料
        q.push(std::move(f));
    }

    // inference：pop 全部，用 span 讀
    while (!q.empty()) {
        Frame f = q.pop();
        process(std::span<const char>(f.data(), f.get_size()));
    }
}
```

跑起來，確認：
- 每個 frame alloc 一次、free 一次
- 沒有 copy（只有 move log）
- ASan 無報錯

---

## 面試常考追問

- 為什麼這裡用 `unique_ptr` 而不是手寫 Rule of Five？（Rule of Zero）
- `push(Frame&& f)` 為什麼參數是 `&&`？傳 `Frame f` 有什麼問題？
- `frames.reserve(16)` 在建構子裡，如果不寫會怎樣？
- 這個 queue 改成多執行緒需要加什麼？（預告：`std::mutex`、`std::condition_variable`）
