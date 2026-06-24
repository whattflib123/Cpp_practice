# 題 D ⭐⭐⭐｜Buffer —— Copy 與 Move 的總驗收

---

## 規格

一個 class 叫 `Buffer`

管理一塊配置在 heap 上的整數陣列。

---

### 成員

至少需要能記錄：

* 陣列指標
* 陣列大小

例如：

```cpp
int* data;
size_t size;
```

（不一定只能這樣設計）

---

### 一般建構子

收一個大小：

```cpp
Buffer buf(5);
```

配置一塊可以放 5 個 int 的陣列。

---

### fill(int value)

把整個陣列填滿。

例如：

```cpp
buf.fill(7);
```

陣列內容變成：

```text
7 7 7 7 7
```

---

### print()

輸出所有元素。

例如：

```text
7 7 7 7 7
```

---

### 解構子

釋放自己擁有的陣列。

---

## Copy Constructor

當發生：

```cpp
Buffer b = a;
```

必須做 **深複製 (Deep Copy)**

也就是：

* 配置新的陣列
* 把內容逐一複製過去

之後：

```cpp
a
```

和

```cpp
b
```

各自擁有自己的記憶體。

互不影響。

---

### 驗證範例

```cpp
Buffer a(3);
a.fill(10);

Buffer b = a;
```

此時：

```text
a: 10 10 10
b: 10 10 10
```

---

## Move Constructor

當發生：

```cpp
Buffer b = std::move(a);
```

必須做 **Move**

也就是：

* 把資源所有權轉給 b
* 不配置新的陣列
* 不複製內容

搬完之後：

```cpp
a
```

必須變成安全的空狀態。

例如：

```cpp
data = nullptr;
size = 0;
```

---

### 驗證範例

```cpp
Buffer a(3);
a.fill(5);

Buffer b = std::move(a);
```

結果：

```text
b: 5 5 5
```

而：

```cpp
a
```

已經不再擁有那塊記憶體。

---

## 要能跑出

```cpp
int main()
{
    Buffer a(5);
    a.fill(1);

    std::cout << "a: ";
    a.print();

    Buffer b = a;

    std::cout << "b(copy): ";
    b.print();

    Buffer c = std::move(a);

    std::cout << "c(move): ";
    c.print();

    return 0;
}
```

---

## 完成後你應該能回答

當看到：

```cpp
Buffer b = a;
```

你腦中要能立刻想到：

```text
Copy Constructor
深複製
兩份資源
```

而看到：

```cpp
Buffer b = std::move(a);
```

要立刻想到：

```text
Move Constructor
資源過戶
舊物件被搬空
不複製資料
```

這就是 C++ 資源管理最重要的分水嶺之一。
