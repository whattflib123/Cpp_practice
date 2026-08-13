# Stage 2 — 指標與陣列運算

## 場景

V4L2 driver 回傳裸 buffer：`width=6, height=4, stride=8`
stride > width → 每行尾巴有 2 bytes padding（driver 對齊用）。

## 核心公式

```cpp
// 第 y 行第 x 欄的元素
buf[y * stride + x]

// 整塊初始化
std::fill(buf, buf + stride * height, 0xFF);

// 只走有效像素
for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)   // width，不是 stride
        use(buf[y * stride + x]);
```

## Stride Bug（面試常考）

內圈誤用 stride 當上限 → padding 的 0xFF 也被加進去：
- 正確 average（width=6）：17.5
- 錯誤 average（stride=8）：102.5

數值「看起來怪」但不崩潰，是 perception pipeline 最難察覺的 bug 之一。

## Array Decay

```cpp
unsigned char arr[32];
sizeof(arr);           // 32 ← 陣列實體
unsigned char* p = arr;
sizeof(p);             // 8  ← 指標，長度資訊消失
```

`new` 回傳的是指標，所以 `sizeof(buf)` 永遠是 8（64-bit）。
長度需要另外傳參數，這就是為什麼函式簽名要帶 `width / height / stride`。

## Pointer Arithmetic 步長

步長由**型別大小**決定：
- `unsigned char*` 移 1 步 = 1 byte
- `int*` 移 1 步 = 4 bytes

所以 `buf + y * stride + x` 在 `unsigned char*` 時才是 byte-level 定址。

## 驗收問題（已過）

1. `buf` 改成 `int*` → 步長變 4，算出的位址不同，byte offset 差 4 倍
2. 兩個 `sizeof` 都印 8 → array decay，指標不帶長度
3. `average` 用 `const unsigned char*` → 唯讀借用，不複製，不改原始資料
4. 不能省 stride → 有效像素之後有 padding，行尾地址要靠 stride 跳

## 待完成

- ASan 越界測試：`buf[height * stride] = 0;` 取消註解，觀察報告
