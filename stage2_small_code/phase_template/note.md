# Stage 13 — Template 基礎

## 微題清單

- [x] 13-1：Function template，型別推導
- [x] 13-2：Class template，`Pair<T, U>`
- [x] 13-3：型別推導實驗，`auto` vs template 推導規則對比
- [x] 13-4：`T&&` vs `Type&&`，forwarding reference vs rvalue reference
- [x] 13-5：`std::forward` 完美轉發
- [x] 13-6：整合，`make_frame<T>(args...)` 模擬 perception pipeline factory

---

## 13-1：Function template，型別推導

**目標**：寫一個能比較任意型別大小的 `my_max`，觀察編譯器型別推導。

**規格**：

```cpp
template <typename T>
T my_max(T a, T b) {
    // 回傳較大的那個
}
```

`main` 裡呼叫三次：
```cpp
my_max(3, 5);       // int
my_max(1.2, 0.8);   // double
my_max('a', 'z');   // char
```

每次印出結果和型別（用 `typeid(T).name()` 印型別名）。

需要：
```cpp
#include <typeinfo>
```

編譯：
```
g++ -std=c++20 -Wall -Wextra -g -fsanitize=address main.cpp -o main && ./main
```

跑完回答：
> 三次呼叫都沒有寫 `my_max<int>(...)`，編譯器怎麼知道 `T` 是什麼？

---

## 13-2：Class template，`Pair<T, U>`

**目標**：手刻一個能裝兩種不同型別的 `Pair`。

**規格**：

```cpp
template <typename T, typename U>
class Pair {
    // 存 first（型別 T）和 second（型別 U）
    // 提供 ctor 和 print() method
};
```

使用：
```cpp
Pair<int, std::string> p1(42, "hello");
Pair<float, char> p2(3.14f, 'x');
p1.print();
p2.print();
```

---

## 13-3：型別推導實驗

**目標**：觀察 `auto` 和 template 推導的共同規則——都會丟掉 reference 和 top-level const。

**規格**：

```cpp
template <typename T>
void show_type(T x) {
    std::cout << typeid(T).name() << "\n";
}

int main() {
    int a = 1;
    const int b = 2;
    int& r = a;

    show_type(a);   // T 推導成什麼？
    show_type(b);   // const 保留嗎？
    show_type(r);   // & 保留嗎？
}
```

跑完回答：
> `const` 和 `&` 都丟掉了，這和 `auto` 推導有什麼關係？

---

## 13-4：`T&&` vs `Type&&`

**目標**：區分 forwarding reference 和 rvalue reference，這兩個長得一樣但完全不同。

**規格**：

```cpp
// A：rvalue reference（Type 固定）
void foo(int&& x) {
    std::cout << "rvalue ref: " << x << "\n";
}

// B：forwarding reference（T 是 template 參數）
template <typename T>
void bar(T&& x) {
    std::cout << "forwarding ref\n";
}

int main() {
    int a = 5;

    // foo(a);       // 能不能編譯？
    foo(5);          // 能不能編譯？
    bar(a);          // 能不能編譯？
    bar(5);          // 能不能編譯？
}
```

把 `foo(a)` 取消註解試試，觀察報錯。

跑完回答：
> `foo` 和 `bar` 都寫 `&&`，為什麼 `bar(a)` 可以接 lvalue，`foo(a)` 不行？

---

## 13-5：`std::forward` 完美轉發

**目標**：觀察不用 `std::forward` 時，rvalue 傳進 wrapper 後變成 lvalue 的問題。

**規格**：

```cpp
void process(int& x)  { std::cout << "lvalue: " << x << "\n"; }
void process(int&& x) { std::cout << "rvalue: " << x << "\n"; }

// 不用 forward 的 wrapper
template <typename T>
void wrapper_bad(T&& x) {
    process(x);              // x 在這裡是 lvalue（有名字）
}

// 用 forward 的 wrapper
template <typename T>
void wrapper_good(T&& x) {
    process(std::forward<T>(x));   // 保留原本的值類別
}

int main() {
    int a = 10;
    wrapper_bad(a);    // 傳 lvalue
    wrapper_bad(42);   // 傳 rvalue，但 wrapper_bad 裡變成 lvalue

    wrapper_good(a);   // 傳 lvalue
    wrapper_good(42);  // 傳 rvalue，wrapper_good 保留 rvalue
}
```

跑完回答：
> `wrapper_bad(42)` 印出的是 lvalue 還是 rvalue？為什麼？

---

## 13-6：整合，`make_frame` factory

**目標**：用 perfect forwarding 實作一個 `make_frame<T>` factory，模擬 `std::make_unique` 的原理。

**規格**：

```cpp
struct Frame {
    int id;
    std::string name;
    Frame(int i, std::string n) : id(i), name(std::move(n)) {
        std::cout << "Frame ctor: " << id << " " << name << "\n";
    }
};

template <typename T, typename... Args>
T make_obj(Args&&... args) {
    return T(std::forward<Args>(args)...);
}

int main() {
    auto f = make_obj<Frame>(1, "cam0");
}
```

跑完回答：
> `make_obj` 裡的 `Args&&...` 是什麼？`std::forward<Args>(args)...` 在做什麼？

---

## 關鍵觀念（待補完）
