
## 題 B:`IntStack`

### 規格
一個簡化的「堆疊」(後進的放最上面),用陣列實作:

1. private:`int* data`(陣列)、`int capacity`(總容量)、`int count`(目前塞了幾個)。
2. 建構子:收一個 `capacity`,在 heap 配置這麼大的 int 陣列,`count` 初始為 0。
3. 解構子:`delete[]` + 印 `"stack freed"`(記得守衛)。
4. `push(int value)`:把 value 放到目前的下一個空位,然後 count +1。
5. `top()`:回傳最後放進去的那個(也就是第 `count - 1` 格)。
6. `getSize()`:回傳 count。

### 要能跑出
```cpp
int main() {
    IntStack s(10);          // 容量 10 的 stack
    s.push(100);
    s.push(200);
    s.push(300);
    std::cout << s.top() << std::endl;       // 300(最後放的)
    std::cout << s.getSize() << std::endl;   // 3
    return 0;
}   // 印出 "stack freed"
```

