# Stage 10 — Mini Perception Pipeline

## 這關學什麼

Stage 2~9 的觀念都是孤立練習題。這關把它們串成一個完整的 AMR perception pipeline，
讓你能在面試裡指著真實結構說「這裡我用 RAII 是因為…」「這裡用 move 是因為…」。

**路徑**：先用 Python 寫原型（邏輯清楚，語言不擋路），再用 C++ 重現，
對比兩版本的差異——差異點就是 C++ 面試的考點。

---

## Pipeline 架構

```
Camera (30fps)
  ↓ capture() → Frame（帶 DMA buffer）
  ↓
FrameQueue（thread-safe，capture thread → inference thread）
  ↓
InferenceEngine::run(std::span<float>)
  ↓
std::vector<Detection>（bbox + confidence）
  ↓
Planning module 取用
```

---

## 微題階梯

| 題號 | 檔案 | 內容 | 對應 Stage |
|---|---|---|---|
| 10-1 | `prototype.py` | Python 單執行緒原型 | 邏輯基準 |
| 10-2 | `prototype.py` | 加 producer-consumer thread | Stage 9 |
| 10-3 | `main.cpp` | `DmaBuffer`：RAII + Rule of Five | Stage 4~5 |
| 10-4 | `main.cpp` | `Frame`：包住 DmaBuffer，move-only | Stage 3~4 |
| 10-5 | `main.cpp` | `InferenceEngine::run(std::span<float>)` | Stage 6 |
| 10-6 | `main.cpp` | `FrameQueue` + 整合 producer-consumer | Stage 7~9 |

---

## Python vs C++ 對比（面試核心）

| Python 幫你做的事 | C++ 要自己處理 | 考點 |
|---|---|---|
| GC 自動回收 buffer | RAII destructor 釋放 | Rule of Five、RAII |
| 物件傳遞預設 reference | copy vs move 要明確選 | move semantics |
| list 自動長大 | `reserve()` 避免 reallocation | iterator invalidation |
| GIL 限制真並行 | mutex + cv 保護共享資料 | data race、TSan |
| `bytes` 切片是 copy | `std::span` 是 zero-copy borrow | span lifetime |

---

## 語法參考卡（C++ 部分）

### DmaBuffer（RAII + Rule of Five）

```cpp
class DmaBuffer {
    void* ptr;
    size_t size;
public:
    explicit DmaBuffer(size_t n) : ptr(malloc(n)), size(n) {}
    ~DmaBuffer() { free(ptr); }

    DmaBuffer(const DmaBuffer&) = delete;             // 禁止 copy
    DmaBuffer& operator=(const DmaBuffer&) = delete;

    DmaBuffer(DmaBuffer&& o) noexcept                 // move constructor
        : ptr(o.ptr), size(o.size) { o.ptr = nullptr; o.size = 0; }
    DmaBuffer& operator=(DmaBuffer&& o) noexcept {    // move assignment
        if (this != &o) { free(ptr); ptr = o.ptr; size = o.size;
                          o.ptr = nullptr; o.size = 0; }
        return *this;
    }

    void* data() const { return ptr; }
    size_t bytes() const { return size; }
};
```

### span 借用語意

```cpp
// InferenceEngine 不擁有資料，只借用
void run(std::span<float> pixels) {
    // 直接讀 pixels[i]，不複製
}

// 呼叫端：frame 的 buffer 傳 span，不做 copy
engine.run(std::span<float>(
    reinterpret_cast<float*>(frame.buf.data()),
    frame.buf.bytes() / sizeof(float)
));
```

---

## 面試敘事範本

> 「我做了一個 AMR 的 perception pipeline。Camera capture 產生的 Frame
> 帶著 DMA buffer，用 RAII 管理——DmaBuffer 的 destructor 負責 free，
> 不會 leak。Frame 之間傳遞用 move，不做 copy，因為 DMA buffer 是 move-only。
> Inference engine 接的是 `std::span<float>`，zero-copy 借用 frame 的資料，
> 不另外分配記憶體。Queue 用 mutex + condition_variable，producer 和 consumer
> 跑在不同 thread，用 TSan 驗證沒有 data race。」
