# Stage 9~11 進度

最後更新：2026-08-21

## Stage 9 —— 多執行緒 producer-consumer frame queue

**目標**：把 Stage 8 的單執行緒 queue 加上 `std::mutex` + `std::condition_variable`，
用 TSan 抓 race condition，親眼看到 data race 再修掉。

**狀態**：未開始

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
