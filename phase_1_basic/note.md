# C++ 第一章

## 一、C++ vs Python
- Python 自動管理記憶體(GC)，開發便利，但 GC 會造成不可預期的延遲與較大的 runtime overhead，難以控制記憶體分配時機與位置
- C++ 讓使用者手動管理記憶體佈局（stack/heap/對齊），實現 deterministic latency，適合資源受限的 AI 加速器與 edge device 部署
---
基本上 語法都從右邊往左看比較好閱讀
## 二、Pointer `*`
> 就是一個存位址的東西

### 1. basic
```cpp=
int age = 25;
int* p = &age;
```
`&age`: address of age，代表age的address
`int* p`: p is a pointer to int
所以`int* p = &age;`是 p is a pointer to int, **initialize to** address of age
```cpp
std::cout << age;    // 印出 25     ← 值
std::cout << &age;   // 印出 0x7ffe… ← 它的櫃子編號(位址)

std::cout << p;    // 印出位址(age 住哪)
std::cout << *p;   // 印出 25(順著位址,去拿櫃子裡的東西)
```
- 用置物櫃比喻收尾:
    * age = 櫃子裡的東西(25)
    * &age = 櫃子的編號
    * p = 一張寫著編號的便條紙
    * *p = 拿著便條紙跑去那個櫃子、把裡面的東西拿出來
- 練習題:
    1. 我寫 `int x = 100; int* ptr = &x;` —— 這時候 ptr 裡面裝的是什麼?
    2. 接上題,*ptr 會得到什麼?
    3. &x 代表什麼?
- **Answer**: 
    1. x 的 address 
    2. 100 
    3. address of x

### 2. 指標重要用法
> 讓函式可以去修改傳進來的值
```cpp
void addOne(int* p) {    // 收一個位址
    *p = *p + 1;         // 順著位址,把櫃子裡的東西 +1
}

int addOne2(int num) {    // 收一個變數
    num = num + 1;        // 變數會複製一份出來 不會修改本人
    return num; 
}

int main() {
    int x = 5;
    
    addOne2(x);          // 把 x 的"副本"傳進去
    std::cout << x;      // 這次印出 5
    
    
    addOne(&x);          // 把 x 的"位址"傳進去
    std::cout << x;      // 這次印出 6
}
```
- **指標能讓函式能伸手去改外面的東西,而不只是改一份影本。**
- 可以省記憶體。

3. `**` -- 指向指標的指標
    ```cpp
    int x = 5;
    int* p = &x;     // p 是指標,指向 int

    int** pp = &p;    // pp 是「指標的指標」,指向 p
    ```

    - 通常要做兩次解參考才能拿到最終的值：
        ```cpp
        std::cout << *p;     // 5(p 解一次 → x 的值)
        std::cout << **pp;   // 5(pp 解一次拿到 p,再解一次拿到 x 的值)
        std::cout << *pp;    // 印出 p 的地址(只解一次,拿到的是「p 這個便條」,還是個地址)
        ```
    - 用在 "要改指標本身的位址" 時：
        ```cpp
        void allocate(int* ptr) {     // 直接傳指標進來(沒有再多一層 *)
            ptr = new int(42);        // 把 ptr 改成指向新配置的記憶體
        }

        int main() {
            int* p = nullptr;          // p 一開始指向「沒有」
            allocate(p);                // 把 p 傳進去
            std::cout << *p;            // ❌ 崩潰!p 還是 nullptr,函式沒改到它!
        }
        ```
        透過以下寫法可以更改到指標本身的位址：
        ```cpp 
        void allocate(int** ptr) {       // 收「p 的位址」,所以參數要多一個 *
            *ptr = new int(42);          // 解一次參考,找到「真正的 p」,把它改成指向新記憶體
        }

        int main() {
            int* p = nullptr;
            allocate(&p);                  // 傳 p 的「位址」(&p),不是 p 本身!
            std::cout << *p;               // ✓ 42!p 真的被改了
        }
        ```

## 3. 參考 `&` 的用法
> 參考就是「幫一個已經存在的變數,取一個別名」。

- ### ！！注意 `&` 本身有兩個意思 
    ```cpp
    int x = 5;

    int* p = &x;     // & 後面是「變數」→ 取位址 (address-of)
    int& r = x;      // & 前面是「型別」→ 宣告參考 (reference)
    ```
    > 這章節教的是 參考

#### 舉例：
```cpp
int score = 99;
int& r = score;    // r 是 score 的別名,從現在起 r 就是 score

r = 0;                  // 改 r
std::cout << score;     // 印出 0!因為 r 跟 score 是同一個櫃子
```
- `int& r = score;` 念作「r is a reference to int, initialize到 score」。
- 動 r 就是動 score,完全沒有「影本」
#### 可以透過**參考**將語法變乾淨

```cpp
int score = 99;
int& r = score;

// -------------------------------------
// 透過參考定義輸入，可以讓函數直接去改參數本人

// 如果是用指標寫:要 & 取址、要 * 解參考,符號一堆
void setToZero(int* v) {
    *v = 0;
}
setToZero(&score);      // 呼叫時要加 &

// 參考版:乾乾淨淨,沒有 * 和 &
void setToZero(int& v) {
    v = 0;              // 直接寫 v,不用 *
}
setToZero(r);       // 呼叫時直接傳,不用 &
```
特點：
- 參考像是「這個人的綽號」——綁定誰就是誰,用起來跟本人沒兩樣,簡潔安全。但它有個限制:一出生就得綁定、而且綁定後不能改綁別人。
- 實務上 modern C++ 的習慣是:能用參考就用參考(簡潔安全)
- 需要「可以換目標、可以為空」的彈性時才用指標。

#### 傳值 vs 傳參考:
***「任何會發生『把值接過來』的地方,都有傳值 vs 傳參考的選擇」***
- 如果要改記憶體裡面的值 就要用參考`&` 
- 如果只是要拿值出來運算 就可以不用

包含 函數、for(), ....
```cpp
void f(int& n);                 // 要改本人 → T&
int  f(int n);                  // 只讀 → 傳值
```

## 三、 `const` 的用法
```cpp
const int* a;        
int* const b;        
const int* const c;  
```
- 統一由右邊往左看
    ```cpp
    const int* a;        // a is a pointer to (const int)
                         // → 指向「const int」的指標

    int* const b;        // b is a (const pointer) to int
                         // → 指向 int 的「const 指標」

    const int* const c;  // c is a (const pointer) to (const int)
                         // → 指向「const int」的「const 指標」
    ```

    1. `const int* a;` : 
        - const int: 不能透過a改指到的address的值
        - 但 a 可以指向別的address
        - 想像a是一個望遠鏡，不能透過視窗改看到的值，但可以調整望遠鏡看的地方
        - ```cpp
            const int* a = &x;
            
            *a = 5;    // ❌ 編譯錯誤:不能「透過 a」改值
            a = &y;    // ✓ 指標本身可以改,換指向 y
            ```        
    2. `int* const b;` : 
        - b is a const pointer，跟上面反過來，不能改b指的address
        - 但可以透過b指標，修改address 的值
    3. `const int* const c;`:
        - 不能改 c 的address，也不能透過c改值



## 四、 C++ 兩種記憶體模型 Stack and Heap
> 如何管理物件的記憶體?
> C++有以下兩種型態作為管理記憶體的模型


### (1) 記憶體示意圖
```
高位址  ┌─────────┐
       │  Stack  │ ← 往下長，自動管理 (FILO)
       │    ↓    │
       │         │
       │    ↑    │
       │  Heap   │ ← 位置不固定(heap allocator自動規劃)、順序不重要，用完歸還
低位址  └─────────┘
```


### (2) Stack
> 「這個物件只在這個 function / scope 活著」
> aka 變數離開大括號就被自動釋放
- 用完就自動釋放資源
- 大小在編譯時就固定、速度快但有記憶體大小限制
```Cpp=
void foo() {
    int x = 5;        // 放在 stack
    Person p("Alice"); // 放在 stack
}                      // 離開這裡，x 和 p 自動銷毀
```
### (3) Heap
> 「這個物件要跨 function 或長時間存在」
* 手動用 `new` 分配，手動用 `delete` 釋放
    * *一個`new`配一個`delete`*
    * *一個`new[]`配一個`delete[]`（陣列版的）*
* 大小在執行時決定，彈性大
* 速度較慢、忘記 delete 就記憶體洩漏
    ```cpp
    void foo() {
        Person* p = new Person("Bob"); // 放在 heap
        delete p;                      
        // 手動釋放，不然洩漏
    }
    ``` 
- 上面那種寫法(new & delete)工程不常見，因為會有洩漏風險
- 如果沒有釋放 指標變數p(是stack) 離開scope後就消失，但物件： `new Person("Bob")` 還佔著記憶體沒人知道他的address 無法被釋出

> 結語：實務上沒人會用 new 跟 delete，難管理容易忘記，透過RAII


## 五、RAII 資源取得即初始化
> RAII是一種設計原則，全稱 Resource Acquisition Is Initialization，透過兩個用法 Constructor / Destructor 自動管理資源

### (1) 什麼是 Constructor / Destructor
* C++ 只有 **class/struct object** 才有 Constructor / Destructor
#### a. Constructor 建構子
> 「class / struct 物件建立時自動執行的特殊函式」

```cpp=
class Dog {
public:
    Dog() {
        std::cout << "Dog born\n";
    }
};
```
- `Dog()` 就是Constructor，**!! 名稱只能跟Class名稱`Dog`一樣 !!**
- 所以只要建立 Dog 這個物件，就會自動執行`Dog()`
- 跟python class 中的 `__init__` 一樣
    ```python
    class Dog:
        def __init__(self):
            print("Dog born")
    ```
- 細節：
    - Constuctor 不需要寫 return type，void, int, ...
    - ```cpp 
      void Dog() // 不合法
        ```
#### b. Destructor 解構子
> 「物件死亡時自動呼叫的特殊函式」
- 用法跟 Constructor很像，**!! 名稱也只能跟Class名稱`Dog`一樣，只是前面加個`~` !!**


```cpp
#include <iostream>

class Dog {
public:
    Dog() {
        std::cout << "Dog born\n";
    }

    ~Dog() {
        std::cout << "Dog died\n";
    }
};
```

- 如果呼叫的話最後流程會是什麼：
    ```cpp
    int main() {
        {
          Dog d; // 這裡會執行Consturctor Dog()  
        } // 物件離開大括弧{}，物件Dog d死掉，自動呼叫~Dog()
        ...   
    } 
    ```

### (2) RAII 的真正目的 - 釋放資源
- 危險寫法：就是前面提到的 new / delete
    ```cpp
    #include <iostream>
    int main() {
      int* data = new int[100]; // 有手動分配記憶體 new

      data[0] = 42;

      std::cout << data[0] << std::endl;

      // 但用完沒有 delete → memory leak
    }
    ```
- 正確寫法：使用Class做資源管理
    > - 在現代 C++（尤其 C++11 之後）裡，資源管理的主體幾乎全部都是 class/struct + RAII
    > - 因為只有 class 才能攜帶 constructor / destructor，所以只有 class 才能實現 RAII 模型

    ```cpp
    #include <iostream>

    class IntArray {
    private:
        int* data;

    public:
        // Constructor：自動跟 Heap 取得儲存空間
        IntArray(int size) {
            data = new int[size];
            std::cout << "alloc\n";
        }

        // Destructor：結束後自動釋放資源
        ~IntArray() {
            delete[] data;
            std::cout << "free\n";
        }
        
        // 這是一種重新定義運算子行為，以後再說
        int& operator[](int i) {
            return data[i];
        }
    };

    //使用方式
    int main() {
        {
            IntArray arr(100);   // alloc 記憶體位置

            arr[0] = 42;
            std::cout << arr[0] << std::endl;
        } // ← scope 結束，自動呼叫 destructor → free 記憶體資源
    }

    ```
    
### (3) 應用工具 Smart Pointer
> 每次像前面一樣寫class太麻煩，C++則將其打包開發出 Smart Pointer
> 借用Stack的自動釋放記憶體的特性，將 Heap 指標 包進一個 Stack 物件
> -> 就可以讓指標一離開scope 自動被刪除

#### A. unique_ptr：單一所有權
```cpp=
{
    std::unique_ptr<int> p = std::make_unique<int>(5);
} 
```
-  建立一個「擁有 int 的 smart pointer」
    - `p`：一個變數（在 stack 上）
    - 型別：`std::unique_ptr<int>`
    - 初始值：`make_unique<int>(5)` 的結果
- `make_unique<int>(5)`
    - 在 heap 建立一個 heap memory，值為 int 5
    - **unique**: 這塊記憶體只能歸給一個人管
    - 要轉手只能透過 `std::move` 移交管理權
- `p = std::make_unique<int>(5)`
    - 把這個 int 5 的 pointer「交給 p 管」
    - 並且 **p 成為這塊 memory 的唯一 owner**
- scope 結束:
    -  p 的 destructor 被呼叫
    -  pointer被刪除、heap memory被釋放

#### B. shared_ptr：共享所有權
> 每宣告一個，計數器就加1，用完一個計數器就減1
> 到0之後自動刪除

```cpp
std::shared_ptr<int> a = std::make_shared<int>(42);  // 計數 = 1
std::shared_ptr<int> b = a;   // ✓ 可以複製!計數 = 2
// a 消失 → 計數降回 1,記憶體不釋放(b 還在用)
// b 也消失 → 計數歸 0 → 此刻才自動釋放
```
