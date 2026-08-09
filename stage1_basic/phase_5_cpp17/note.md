# C++第五章 (Cpp-17)
> C++17 讓code更乾淨的版本

## 一、結構化綁定(structured bindings)
> 就很像python了

### 說明：
假設 `pair` 有兩個值 `first`, `second`

```cpp
for (auto& pair : ages) {
    std::cout << pair.first << ": " << pair.second << "\n";
    //            ↑ key            ↑ value
}

// 可以將打包的東西拆掉 -> 將 pair 拆成自己命名的 key, value
for (auto& [key, value] : ages) {       // 一次拆成 key 和 value
    std::cout << key << ": " << value << "\n";
}

```
- return 
    - 以前只能return 一個值，現在可以拆開回傳
    - 也可以拆開接回這些值
    ```cpp
    std::pair<int, int> minMax(int a, int b) {
        if (a < b) return {a, b};       // {較小, 較大}
        else       return {b, a};
    }

    int main() {
        auto [lo, hi] = minMax(8, 3);   // 一次接住兩個回傳值
        std::cout << lo << " " << hi;   // 3 8
    }
    ```
    
## 二、`std::optional` —— 「可能有、可能沒有」的安全表達
以前會用一個實際的值代表 空物件，例如`-1`, ..

現在透過`std::optional<T>`，回傳要有T 或傳空的 `std::nullopt`
```cpp
#include <optional>

std::optional<int> findScore(std::string name) {
    if (name == "Alice") {
        return 90;              // 有 → 回傳值
    }
    return std::nullopt;        // 沒有 → 明確回傳「空」
}

// 使用：
auto result = findScore("Alice");

if (result.has_value()) {           // 問:裡面有東西嗎?
    std::cout << result.value();    // 有 → 用 .value() 取出來 → 90
} else {
    std::cout << "查無此人";
}

```
- `std::optional<int>`，可能有 int 也可能沒有東西

--- 
## `auto`
auto 的意思:「型別你(編譯器)自己推斷,我懶得寫。」
```cpp
auto x = 5;          // 編譯器看到 5 是 int → x 就是 int
auto y = 3.14;       // 看到 3.14 是 double → y 是 double
auto s = "hello";    // 推斷成字串型別

// ==========================================================
// 真正好用
// 不用 auto,你得寫這一長串:
std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();

// 用 auto,編譯器自己推斷:
auto end_time = std::chrono::high_resolution_clock::now();
```
**如果寫死型別能讓讀程式碼的人一眼看懂,那就寫死**

---

## 三、`std::string_view`  —— 「只看不複製」的字串
> 只想「讀」一個字串、不想「複製」它的時候,避免昂貴的複製。

- 以前的標準作法：`const`+傳參考，不傳值
    ```cpp
    void f(const std::string& s);   // const 參考:不複製、只讀 ← 你學過的「標準答案」
    ```
- 但有些寫法會偷偷複製出字串來：
    ```cpp
    // 偷偷複製的情況

    void printIt(const std::string& s) { 
        std::cout << s; 
    }

    printIt("hello");   // ← 這裡其實偷偷做了一次複製!
    ```
- `std::string_view` 解決這個:它是一個「字串的觀景窗」——只記住「字串在哪、多長」,完全不複製內容。
    ```cpp
    // 避免的改法

    #include <string_view>
    void printIt(std::string_view s) {    // 用 string_view 收
        std::cout << s;
    }

    printIt("hello");          // ✓ 不複製,只是「看著」"hello"
    std::string name = "Bob";
    printIt(name);             // ✓ 也不複製,只是「看著」name
    ```
    - 限制(一定要知道,否則會出 bug):string_view 只是「借看」,它不擁有資料
    - 所以 `string_view` 最適合當函式參數(用完即走,原字串還活著),不適合長期保存。
        ```
        只讀字串、且生命週期短(函式參數)→ std::string_view(最快,零複製)
        需要「擁有」「保存」「修改」字串    → std::string
        要改呼叫者的字串                  → std::string&
        ```