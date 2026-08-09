
## 題 C:`Logger`

### 規格
一個記錄器,用 RAII 自動印開始/結束,中間統計呼叫次數:

1. private:一個 `int log_count`(記錄呼叫幾次),**沒有任何指標**。
2. 建構子:`log_count` 設為 0,印 `"=== log start ==="`。
3. `log()`:`log_count` 加 1(模擬記錄一筆)。
4. 解構子:印 `"=== log end, total: X ==="`(X 是總共呼叫幾次)。

### 要能跑出
```cpp
int main() {
    {
        Logger lg;          // 印 "=== log start ==="
        lg.log();
        lg.log();
        lg.log();
    }   // lg 離開 scope → 印 "=== log end, total: 3 ==="

    std::cout << "main done" << std::endl;
    return 0;
}
```

預期輸出:
```
=== log start ===
=== log end, total: 3 ===
main done
```

