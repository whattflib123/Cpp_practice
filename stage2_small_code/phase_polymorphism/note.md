# Stage 12 — 多型 / virtual / vtable

## 微題清單

- [x] 12-1：`virtual` function 基礎，派生類別覆寫，印出誰被呼叫
- [x] 12-2：vtable 可觀察實驗，`sizeof` 對比有/無 virtual，看出 vptr 8 bytes
- [x] 12-3：pure virtual / abstract class，interface 模式
- [x] 12-4：`virtual` destructor，不加會怎樣（ASan 抓 leak）
- [x] 12-5：`override` 關鍵字，故意拼錯函式名，有/無 `override` 編譯期行為對比
- [x] 12-6：整合，`unique_ptr<Base>` 管 derived，模擬 InferenceBackend interface

---

## 核心觀念

### virtual function — 編譯期 vs 執行期派發

```cpp
InferenceBackend* p = &trt;
p->run();
```

- **沒有 `virtual`**：編譯期根據 `p` 的靜態型別（`InferenceBackend*`）決定呼叫哪個函式。永遠呼叫 base 版本。
- **有 `virtual`**：執行期根據物件的**實際型別**查 vtable，呼叫正確的 derived 版本。

面試常考：「指標型別 vs 物件實際型別的差別」就是這個。

---

### vtable 機制

每個有 `virtual` function 的 class，編譯器自動產生一張 **vtable**（函式指標陣列）。
每個物件有一個隱藏的 **vptr**（8 bytes，指向 vtable）。

```
sizeof(NoBirtual)  = 4   // 只有 int
sizeof(WithVirtual) = 16  // int(4) + padding(4) + vptr(8)
```

`p->run()` 的實際機器碼：`*(p->vptr[0])(p)`，多一次間接跳轉。

---

### pure virtual / abstract class

```cpp
class InferenceBackend {
public:
    virtual void run(std::span<float> data) = 0;  // pure virtual
    virtual ~InferenceBackend() = default;
};
```

- `= 0`：強制 derived class 實作，否則無法具現化。
- 有任何 pure virtual → class 是 **abstract class**，`new InferenceBackend` 編譯期報錯。
- 用途：定義介面合約，呼叫方只依賴 base pointer，不知道也不在意底層是 TrtBackend 還是 CpuBackend。

---

### virtual destructor — 為什麼必須加

```cpp
InferenceBackend* p = new TrtBackend();
delete p;
```

- **沒有 `virtual ~InferenceBackend()`**：只呼叫 `InferenceBackend::~InferenceBackend()`，`TrtBackend` 的成員（如 `unique_ptr`）沒有被正確銷毀 → **undefined behavior / memory leak**。
- **有 `virtual ~InferenceBackend()`**：執行期查 vtable，呼叫 `TrtBackend::~TrtBackend()`，再往上呼叫 base dtor。

**規則**：base class 有 `virtual` function → dtor 也一定要 `virtual`。

---

### `override` 關鍵字

```cpp
class TrtBackend : public InferenceBackend {
public:
    void run(std::span<float> data) override;  // 明確宣告覆寫
};
```

- 沒有 `override`：拼錯函式名（如 `Run`）→ 靜默建立新函式，不覆寫 base，bug 藏很深。
- 有 `override`：簽名不符 → **編譯期報錯**。
- 實務上 derived class 所有覆寫都加 `override`，把執行期 bug 提前到編譯期。

---

### `unique_ptr<Base>` 管 derived — 正確用法

```cpp
std::unique_ptr<InferenceBackend> backend = std::make_unique<TrtBackend>();
backend->run(data);  // 執行期多型
```

- 所有權由 `unique_ptr` 管，離開 scope 自動 delete。
- delete 時要呼叫正確的 dtor → **base dtor 必須是 virtual**。
- 這是 perception pipeline 常見 pattern：factory 回傳 `unique_ptr<InferenceBackend>`，呼叫方不知道底層實作。

---

## 面試常問

- 「vtable 是什麼，virtual call 比 non-virtual 多做了什麼？」→ vptr 間接跳轉，執行期查表
- 「為什麼 base class destructor 要是 virtual？」→ `delete base_ptr` 時才能呼叫到 derived dtor
- 「pure virtual 和 virtual 的差別？」→ pure virtual 強制 derived 實作，class 變 abstract
- 「`override` 有什麼好處？」→ 把簽名不符的 bug 從執行期提前到編譯期
