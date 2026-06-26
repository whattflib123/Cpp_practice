# C++第四章 - Template
> 為了避免函數邏輯一樣，只是輸入型別不同的狀況

## 1. 說明
```cpp
int maxOf(int a, int b) {
    return (a > b) ? a : b;
}
```
若今天要比 `double`，邏輯一樣，只有型別不同
```cpp
double maxOf(double a, double b) {
    return (a > b) ? a : b;    // 邏輯跟 int 版完全一樣!
}
```
--- 
## 2. *Cpp選擇把型別也變成一種參數*

```cpp
template <typename T>
T maxOf(T a, T b) {
    return (a > b) ? a : b;
}
```
- template <typename T> —— 宣告「接下來這個函式有一個型別參數,稱之為 T」。T 是一個佔位符,代表「某個還沒決定的型別」。
- T maxOf(T a, T b) —— 把原本寫 int 的地方全換成 T:回傳 T、參數是兩個 T。意思是「a、b 是某個型別 T,回傳也是 T」。
- T 是命名習慣，使用時編譯器會自己幫你填入型別
    ```cpp
    maxOf(3, 5);        // 傳的是 int → 編譯器把 T 當成 int → 等於呼叫 int 版
    maxOf(3.5, 2.1);    // 傳的是 double → T 變 double → 等於 double 版
    maxOf('a', 'z');    // 傳的是 char → T 變 char → char 版
    ```

## 3. 強大應用 - 結合 Class
> 之前用的 `std::unique_ptr<int>`, std::vector<int>`, ... <>背後就是template
    
以 Buffer 為例：將需要替換的型別換成T就好
```cpp
template <typename T>      // ← 宣告型別參數 T
class Buffer {
private:
    T* data;               // int* → T*(裝的是 T)
    int size;              // 注意:size 還是 int!(見下面說明)
public:
    Buffer(int s) {
        size = s;
        data = new T[s]();  // new int[] → new T[]
    }
    ~Buffer() {
        if (data != nullptr) delete[] data;
    }
    T& operator[](int i) {  // int& → T&(回傳的是 T 的參考)
        return data[i];
    }
};
    
Buffer<int> a(5);        // T = int → 一個裝 int 的 buffer
Buffer<double> b(5);     // T = double → 裝 double 的 buffer
Buffer<Vec2> c(5);       // T = Vec2 → 裝 Vec2 的 buffer!

a[0] = 100;              // int
b[0] = 3.14;             // double
```