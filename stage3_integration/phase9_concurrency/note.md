# Stage 9 — 多執行緒 producer-consumer frame queue

## 這關學什麼

Stage 8 的 queue 是單執行緒：push 完才 pop。
真實 perception pipeline 是兩個執行緒同時跑：

```
Thread A (capture):    push Frame → queue
Thread B (inference):  pop Frame ← queue
```

兩個執行緒共用同一個 queue，沒有保護就是 data race。

---

## 新工具

| 工具 | 用途 |
|---|---|
| `std::thread` | 建立執行緒 |
| `std::mutex` | 互斥鎖，同一時間只有一個執行緒進臨界區 |
| `std::lock_guard` | RAII mutex wrapper，scope 結束自動 unlock |
| `std::condition_variable` | 讓 consumer 等待直到 queue 非空，不忙等 |
| TSan | Thread Sanitizer，執行期抓 data race |

## TSan 編譯指令

```bash
g++ -std=c++20 -Wall -Wextra -g -fsanitize=thread main.cpp -o main
./main
```

**注意**：TSan 和 ASan 不能同時開，分開測試。

---

## 語法參考卡

```cpp
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

// lock_guard — 自動 unlock
{
    std::lock_guard<std::mutex> g(mtx);
    // 臨界區
}

// unique_lock — condition_variable 需要用這個（可以暫時 unlock）
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [] { return ready; });  // 等到 ready == true
    // 條件滿足後繼續
}

cv.notify_one();   // 喚醒一個等待的執行緒
cv.notify_all();   // 喚醒所有等待的執行緒

// 建立執行緒
std::thread t([] { /* 工作 */ });
t.join();  // 等執行緒結束
```

---

## 微題階梯

### 題 9-1：故意製造 data race，用 TSan 抓

兩個執行緒同時對同一個 `int counter` 做 `counter++`，不加任何鎖。

```cpp
int counter = 0;

void increment() {
    for (int i = 0; i < 10000; i++) counter++;
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << counter << '\n';  // 每次結果不同
}
```

跑兩次，結果一樣嗎？再用 TSan 編譯，看報告。

---

### 題 9-2：用 `mutex` 修掉 race

在 `increment` 裡加 `lock_guard`，重跑，確認結果固定是 20000。

---

### 題 9-3：ThreadSafeQueue — push + pop + mutex

把 Stage 8 的 `FrameQueue` 加上 mutex，讓 `push` / `pop` 是執行緒安全的。

```cpp
class ThreadSafeQueue {
    std::vector<Frame> frames;
    std::mutex mtx;
public:
    void push(Frame&& f) {
        std::lock_guard<std::mutex> g(mtx);
        frames.push_back(std::move(f));
    }
    // TODO: pop()
    bool empty() {
        std::lock_guard<std::mutex> g(mtx);
        return frames.empty();
    }
};
```

---

### 題 9-4：加 `condition_variable`，consumer 等待而不忙等

```cpp
// 忙等版（差）
while (q.empty()) { /* spin */ }
Frame f = q.pop();

// condition_variable 版（好）
std::unique_lock<std::mutex> lk(mtx);
cv.wait(lk, [&] { return !frames.empty(); });
```

---

### 題 9-5：完整 producer-consumer pipeline

producer 執行緒 push 5 個 frame，consumer 執行緒 pop 全部並 process，
用 condition_variable 協調，push 完後發送「完成」訊號讓 consumer 結束。

---

### 題 9-6：ThreadPool

N 個 worker thread 共用一個 task queue，main thread enqueue lambda，worker 自動取出執行。
pool 解構時自動 shutdown，等所有 worker 結束。

**介面**：

```cpp
class ThreadPool {
public:
    explicit ThreadPool(int n_threads);
    void enqueue(std::function<void()> task);
    ~ThreadPool();  // shutdown + join all workers
};
```

**worker loop 邏輯**：

```
while (true) {
    等待 task queue 非空或 shutdown
    如果 shutdown 且 queue 空 → 退出
    取出 task → 執行
}
```

**新語法——`std::function`**：

```cpp
#include <functional>
#include <queue>

std::queue<std::function<void()>> tasks;

tasks.push([] { std::cout << "hello\n"; });  // 塞 lambda

auto t = tasks.front();
tasks.pop();
t();  // 執行
```

**注意**：內部結構與 `ThreadSafeQueue` 相同，`frames` 換成 `std::queue<std::function<void()>>`，`set_done()` 搬進 destructor。

---

### ThreadPool destructor：為什麼要 `notify_all()`

pool 有 N 個 worker，task 跑完後**全部**都在 `cv.wait` 裡睡著。
destructor 設 `stop = true` 後，必須喚醒所有人讓他們各自檢查 `stop && empty` 後退出。

```cpp
~ThreadPool() {
    { std::lock_guard<std::mutex> g(mtx); stop = true; }
    cv.notify_all();               // ← 全部喚醒，不是 notify_one
    for (auto& t : workers) t.join();
}
```

`notify_one()` 只叫醒一個 → 其他 N-1 個繼續睡 → `join()` 永遠等不到 → **deadlock**。

### ThreadPool vs ThreadSafeQueue 結構對比

| | ThreadSafeQueue（9-5）| ThreadPool（9-6）|
|---|---|---|
| push | `frames.push_back(Frame)` | `tasks.push(function)` |
| pop | `frames.back()` + `pop_back()` | `tasks.front()` + `pop()` |
| 資料型別 | `Frame` | `std::function<void()>` |
| consumer | 外部 thread | pool 內部 worker threads |
| shutdown | `set_done()` 由外部呼叫 | destructor 自動觸發 |

---

## 面試常考追問

- data race 和 race condition 差在哪？
- `lock_guard` vs `unique_lock`：什麼時候用哪個？
- `condition_variable` 為什麼需要 `unique_lock` 而不是 `lock_guard`？
- spurious wakeup 是什麼？`cv.wait` 的 predicate lambda 為什麼重要？
- 這個 queue 的瓶頸在哪？mutex 鎖住整個 queue 有什麼問題？
- ThreadPool destructor 為什麼用 `notify_all()` 不用 `notify_one()`？
