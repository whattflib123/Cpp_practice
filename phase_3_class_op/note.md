# C++ 第三章 Class進階 - 運算子多載(operator overloading)
> 就是可以在自己定的class中，字定義運算子的行為

例如：
```cpp
class Buffer {
private:
    int* data;
    int size;
public:
    // ... 建構子等 ...

    int& operator[](int i) {    // 定義「對 Buffer 用 [] 時做什麼」
        return data[i];
    }
};
```
- int& —— 回傳型別,參考(這樣 buf[0] = 99 才改得到正本,還記得參考的作用嗎?)
- operator[] —— 函式名,意思是「我定義 [] 這個運算子」
- (int i) —— 參數,使用者寫 buf[2] 時,2 就傳進這個 i
- return data[i]; —— 實際做的事:回傳內部陣列的第 i 格

定義好之後，就可以使用自定義的運算子：
```cpp
Buffer buf(5);
buf[0] = 100;              // 等於呼叫 buf.operator[](0),回傳 data[0] 的參考,再賦值 100
std::cout << buf[0];       // 100
```

### 結論：

```cpp
class Vec2{
    public: // 危險的例如指標，才須放在private，這裡public 可以開放給外面使用
        int x;
        int y;

        Vec2(int x, int y){
            this->x = x;
            this->y = y;
        }
            // operator+:定義「兩個 Vec2 相加」
        Vec2 operator+(const Vec2& other) {
            return Vec2(x + other.x, y + other.y);   // 回傳一個全新的 Vec2
        }

        Vec2 operator-(const Vec2& other){
            return Vec2(x - other.x, y - other.y); 
        }

        bool operator==(const Vec2& other) {
            return (x == other.x) && (y == other.y);
        }  
};

int main(){
    Vec2 a(1, 2);
    Vec2 b(1, 2);
    Vec2 c(5, 5);

    // 相加
    Vec2 c = a + b;        // 等於 a.operator+(b) → 回傳 Vec2(4, 6)
    std::cout << c.x << ", " << c.y;   // 4, 6
    
    // 比較
    if (a == b){
        std::cout << "a 和 b 相等\n";
    }

}
```


1. **總共會有三種回傳的類**
    operator[]  → 回傳 int&（參考）   → 因為要存取「已存在的那一格」
    operator+   → 回傳 Vec2（新物件） → 因為算出一個「新的東西」
    operator==  → 回傳 bool（真假）   → 因為它在「判斷」,答案只有「相等 / 不相等」

    ```cpp
    int&  operator[] (int i)              { ... }   // 回傳 int&,運算子 [],參數 int i
    Vec2  operator+  (const Vec2& other)  { ... }   // 回傳 Vec2,運算子 +,參數 other
    Vec2  operator-  (const Vec2& other)  { ... }   // 回傳 Vec2,運算子 -,參數 other
    bool  operator== (const Vec2& other)  { ... }   // 回傳 bool,運算子 ==,參數 other
    ```


2. 語法：
    ```
    回傳型別  operator運算子(參數)  { ... }
    ```
    
    - 為何像是 +, -, 大小於, ... 的輸入參數只有一個 Vec2  operator+  (const Vec2& other)
        - 因為呼叫者本身可以直接使用，只是被隱藏了 **(隱含左運算元)**
            ```cpp
            左運算元.operator運算子(右運算元)
              ↑                      ↑
             隱含的呼叫者          明寫的參數
             (函式內直接用成員名)  (函式內用 參數名. 存取)
            ---------------------------------------
            a + b
              ↓ 翻譯成
            a.operator+(b)
              ↑          ↑
             左運算元    右運算元(這個才是參數)
             (呼叫者,    
              隱含的)            
            ```