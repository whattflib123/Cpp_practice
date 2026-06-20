
## 題 D:`Buffer`

### 規格
管一塊 int 陣列,**同時支援複製和移動**:

1. private:`int* data`、`int size`。
2. **建構子**:收 `size`,`new int[size]()`,記住 size。
3. **解構子**:守衛 + `delete[]` + 印 `"freed"`。
4. **set(i, value) / get(i)**:讀寫第 i 格。
5. **Copy 建構子** `Buffer(const Buffer& other)`:**深複製**——自己 new 一塊**新的**、把 other 的內容**逐格抄過來**(不是共享指標!)。
6. **Move 建構子** `Buffer(Buffer&& other)`:**過戶**——抄指標、other 設空。

### 核心:copy 和 move 的差別(寫之前一定要想清楚)

```
Copy(深複製):我 new 一塊全新的 → 把對方的內容一格一格抄過來
              → 結束後兩塊獨立的記憶體,各自擁有
Move(過戶)  :我直接拿走對方的指標 → 對方設 nullptr
              → 結束後還是同一塊,只是換主人,對方變空殼
```

### 要能跑出
```cpp
int main() {
    Buffer a(3);
    a.set(0, 100);

    Buffer b = a;              // copy(深複製):b 是獨立的一塊
    b.set(0, 999);             // 改 b 不該影響 a

    std::cout << a.get(0) << std::endl;   // 100(沒被 b 影響 → 證明是深複製)
    std::cout << b.get(0) << std::endl;   // 999

    Buffer c = std::move(a);   // move:c 接管 a 的資源,a 變空殼
    std::cout << c.get(0) << std::endl;   // 100

    return 0;
}
```

