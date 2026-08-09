規格

1. 一個 class 叫 Box,private 有一個 int* data。
2. 建構子收一個 int value,在 heap 配置一個 int 存進去。
3. 解構子:若 data 非空 → delete + 印 "box gone"。
4. set(int value):改掉裡面的值。
5. get():回傳裡面的值。


要能跑出:
```cpp
int main() {
    Box b(10);
    std::cout << b.get() << std::endl;   // 10
    b.set(99);
    std::cout << b.get() << std::endl;   // 99
    return 0;
}   // 印出 "box gone"
```