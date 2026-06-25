 # 題 A：RequestTracker


## 規格

一個 class 叫 `RequestTracker`

private 有一個：

```cpp
int counter;
```

---

### 建構子

建立物件時：

```text
Request started
```

並把計數器設成 0。

---

### log()

每呼叫一次：

```cpp
tracker.log();
```

計數器加 1。

---

### count()

回傳目前累積次數。

---

### 解構子

物件生命週期結束時印出：

```text
Request finished, total logs = X
```

其中 X 為累積次數。

---

## 要能跑出

```cpp
int main()
{
    RequestTracker tracker;

    tracker.log();
    tracker.log();
    tracker.log();

    std::cout << tracker.count() << std::endl;

    return 0;
}
```

輸出：

```text
Request started
3
Request finished, total logs = 3
```

---

## 你要自己寫的（從這個空殼開始）

```cpp
#include <iostream>

class RequestTracker {
    // 自己寫:
    // private 成員
    // 建構子
    // 解構子
    // log()
    // count()
};

int main()
{
    RequestTracker tracker;

    tracker.log();
    tracker.log();
    tracker.log();

    std::cout << tracker.count() << std::endl;

    return 0;
}
```

