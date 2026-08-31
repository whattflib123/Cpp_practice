# Stage 9~11 進度

最後更新：2026-08-28

## Stage 9 —— 多執行緒 producer-consumer frame queue

**目標**：把 Stage 8 的單執行緒 queue 加上 `std::mutex` + `std::condition_variable`，
用 TSan 抓 race condition，親眼看到 data race 再修掉。

**狀態**：✅ 完成（9-1 ~ 9-6）

### 完成的微題
- 9-1：故意製造 data race，TSan 報告確認
- 9-2：`mutex` + `lock_guard` 修掉 race，結果固定 20000
- 9-3：`ThreadSafeQueue` — push/pop 加 mutex
- 9-4：加 `condition_variable`，consumer 改為等待而非忙等
- 9-5：完整 producer-consumer pipeline，`set_done()` 訊號讓 consumer 自動退出

### 關鍵觀念（已考過，仍需複習）
- spurious wakeup：OS 無緣無故喚醒 wait，predicate lambda 是守門人
- `unique_lock` vs `lock_guard`：cv.wait 需要能暫時 unlock，lock_guard 辦不到
- `set_done()` 必須在 producer thread 內最後呼叫，不能從 main thread 呼叫
- 效能瓶頸：單一 mutex 使 push/pop 無法真正並行；改善方向：lock-free queue 或 double-buffer

### 9-6 補完
- ThreadPool：N worker threads + std::function task queue，destructor 自動 shutdown + join
- notify_all() vs notify_one() shutdown 差異已理解

---

## Stage 10 —— Mini Perception Pipeline（替代原 EdgeInferencePool 整合）

**目標**：Python 原型 → C++ 移植，覆蓋 Stage 2~9 全觀念，建立面試敘事。

**狀態**：✅ 完成（10-1 ~ 10-6）

### 完成
- 10-1：Python 單執行緒原型（Camera / Frame / InferenceEngine / Detection）
- 10-2：Python 加 producer-consumer threading（queue.Queue）
- 10-3：C++ `DmaBuffer`——RAII + Rule of Five + lifecycle log
- 10-4：C++ `Frame`——Rule of Zero 包住 DmaBuffer，move-only
- 10-5：`InferenceEngine::run(std::span<float>)`——zero-copy borrow
- 10-6：`FrameQueue` + 整合 producer-consumer，完整 pipeline 兩 thread 跑通

### 面試敘事（已驗收）
- `unique_ptr` 管 buffer → RAII，不 leak，不用手寫 destructor
- `unique_lock` vs `lock_guard` → cv.wait 需要暫時 unlock
- `span` vs `vector&` → zero-copy borrow，接受任何連續記憶體（待加強）

### 檔案
- `stage3_integration/phase10_mini_pipeline/prototype.py`
- `stage3_integration/phase10_mini_pipeline/main.cpp`
- `stage3_integration/phase10_mini_pipeline/note.md`

---

## Stage 11 —— ROS2 sensor callback 所有權模型

**目標**：FactoryPerception：camera/LiDAR frame 的 move/span 設計、多 subscriber 所有權。

**狀態**：未開始

FactoryPerception repo 路徑：（待補）
