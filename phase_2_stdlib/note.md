# C++ 第二章

## 二、Vector and For loop

### 1. 基本用法:
```cpp
#include <vector>

std::vector<int> v;        // 一個空的 int 容器
v.push_back(10);           // 塞一個 10 進去 → {10}
v.push_back(20);           // 再塞 → {10, 20}
v.push_back(30);           // → {10, 20, 30}

std::cout << v[0];         // 10,跟陣列一樣用 [] 取
std::cout << v.size();     // 3,目前裝了幾個
```
- `push_back()`:將元素塞到vector尾巴
### 2. 初始化的寫法
兩種常見：
```cpp
std::vector<int> a = {1, 2, 3}; //直接給值

std::vector<int> b(100, 0); // 初始化100個元素為0
// 等效於 int b = new int[100]; 但這種寫法不好
```

### 3. for
就跟python 很像，只是有傳值跟傳參考的差別
```cpp

std::vector<int> v = {1,2,3};

// ================================================

// 1. 傳值
for(int i : v){
    i = i * 2;
}
for(int j : v){
    std::cout << j << std::endl; // {1,2,3}
}

// ================================================

// 2. 傳參考
for(int& j : v){
    j = j * 2;
}
std::cout << v << std::endl; // {2,4,6}
```

## 三、String
> 跟vector很像，是裝 `char` ，並加入一些`char`特有的語法而已

note: 字串用雙引號`" "`、字元用單引號`' '`
### 1. 基本
基本就跟vector 一樣
```cpp
#include <string>

std::string s = "hello";

s.size();          // 5,長度(跟 vector 一樣)
s[0];              // 'h',取第一個字元(注意:取出來是 char,單引號那種!)
s.push_back('!');  // 尾巴塞一個字元 → "hello!"

for (char c : s) {             // range-based for 也通用
    std::cout << c << " ";     // h e l l o !
}
```

### 2. 常用用法：串接、比較、找子字串
跟python也很像
```cpp
std::string a = "hello";
std::string b = "world";

// 1. 串接:直接用 +
std::string c = a + " " + b;       // "hello world"

// 2. 比較:直接用 ==
if (a == "hello") { ... }          // 直觀,跟 Python 一樣

// 3. 找子字串:find
size_t pos = a.find("ll");         // 回傳 2(從位置 2 開始)
if (a.find("xyz") == std::string::npos) {
    // 找不到時回傳特殊值 npos,用這個判斷「不存在」
}
```

### 3. string 作為參數如何傳
1. 傳值
2. 傳參考
    - 可改值
    - 只讀字串本身的值

```cpp
void f(std::string s);          // 傳值:整個字串複製一份,大字串 = 災難
void f(std::string& s);         // 傳參考:不複製,但函式「可以改」呼叫者的字串
void f(const std::string& s);   // const 參考:不複製 + 保證不改 ← 只讀時的 "標準答案" 最重要!!
```

## 四、move
> 什麼時候要用move
> 「這個物件之後還要不要用?」 還要用 → 不准 move;
> 用不到了(暫存、即將離開 scope)→ move。

- 移交指標的管理權，避免需要複製很大的資料。
    ```cpp
    std::string a = "一篇五萬字的小說...";   // a 管著 heap 上一大塊記憶體
    std::string b = a;                      // 複製:把五萬字逐字抄一份
    std::string c = std::move(a);   // 過戶,不是抄寫
    ```
- 這是結合之前的unique_ptr:
    ```cpp
    std::unique_ptr<int> p = std::make_unique<int>(42);
    std::unique_ptr<int> q = p;              // ❌ 複製,編譯直接擋下
    std::unique_ptr<int> q = std::move(p);   // ✓ 過戶:q 接管,p 變 nullptr
    ```
移交管理員後，原先的變數就沒有使用的權限了，只有真的不需要了才釋放資源
```cpp
int main(){
    std::unique_ptr<int> p = std::make_unique<int>(42);

    std::cout << *p;              // ✓ 42,此刻 p 是主人,能正常使用

    std::unique_ptr<int> q = std::move(p);   // 管理權過戶給 q

    std::cout << *q;              // ✓ 42,現在 q 是主人
    std::cout << *p;              // ❌ 危險!p 已交出管理權,變成 nullptr
                                 //    對 nullptr 解參考 → 程式崩潰
} // ← scope 結束:q 的解構子在這裡執行 → 那塊記憶體在此刻才被釋放
```
## 五、nullptr
當指標沒有指到東西的時候，會顯示nullptr
為了避免程式用到nullptr，實務上常用：
> 對任何指標（裸指標 / unique_ptr / shared_ptr）都通用
```cpp
if (p) {                  // p 非空才進來；等同 if (p != nullptr) 但更簡潔
    std::cout << *p;
} else {
    std::cout << "p 是空的，不能用";
}
```





