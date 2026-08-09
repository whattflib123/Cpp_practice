
## 題:用 `std::map` 數數

### 情境
給你一句話拆成的單字,統計每個單字出現幾次——這是 map 最經典的用法(計數)。

### 規格
```cpp
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> words = {"apple", "banana", "apple", "cherry", "banana", "apple"};

    std::map<std::string, int> count;   // key=單字, value=出現次數

    // TODO: 跑過 words,每個單字在 map 裡 +1

    // 印出每個單字和它的次數
    for (auto& pair : count) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}
```

### 預期輸出
```
apple: 3
banana: 2
cherry: 1
```
