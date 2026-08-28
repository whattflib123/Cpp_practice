# Stage 9~11 進度

最後更新：2026-08-28

## Stage 9 —— 多執行緒 producer-consumer frame queue

**目標**：把 Stage 8 的單執行緒 queue 加上 `std::mutex` + `std::condition_variable`，
用 TSan 抓 race condition，親眼看到 data race 再修掉。

**狀態**：9-1 ~ 9-5 完成

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

### 待確認
- CLAUDE.md Stage 9 提到 ThreadPool 派工，note.md 只到 9-5——是否需要 9-6 ThreadPool？

---

## Stage 10 —— Stage 2~7 觀念搬進 EdgeInferencePool 正式 repo

**目標**：`DpuBuffer`（RAII + Rule of Five）、`InferenceBackend`（span 借用語意）真實實作。

**狀態**：未開始

EdgeInferencePool repo 路徑：（待補）

---

## Stage 11 —— ROS2 sensor callback 所有權模型

**目標**：FactoryPerception：camera/LiDAR frame 的 move/span 設計、多 subscriber 所有權。

**狀態**：未開始

FactoryPerception repo 路徑：（待補）
