# C++第七章 static
根據不同位置有不同意思

## 1. 在 **function** 內：
> 只有第一次呼叫那個函式時初始化

只要用static，就不用宣告一個全域變數。
```cpp
void f() {
    static int count = 0;    // 只在「第一次呼叫」時初始化
    count++;
    std::cout << count;
}

f();    // 1
f();    // 2(記得上次的值!)
f();    // 3
```

## 2. 在 Class 內：
> 所有這個class的物件，共用一個變數

```cpp
class Counter {
public:
    static int totalCount;    // 不屬於任何一個物件,是「整個 class 共用」的
    Counter() {
        totalCount++;          // 每造一個物件,共用的計數就+1
    }
};

int Counter::totalCount = 0;    // static 成員要在 class 外面額外初始化(語法規定)

Counter a, b, c;
std::cout << Counter::totalCount;   // 3(造了三個物件)
```

## 3. 整個檔案：
> 這個東西只有這份檔案可以用，其餘檔案看不到

```cpp
// utils.cpp
static int helperCount = 0;     // 加了 static,只有 utils.cpp 自己能用這個變數
                                  // 別的 .cpp 檔(像 main.cpp)完全看不到、碰不到它
```


## 4. 題目：
**寫一個 class 叫 `IdGenerator`,每次建立一個物件,自動給它一個獨一無二、遞增的 id(第一個物件 id=1、第二個 id=2…)。**

提示:你需要一個 `static int nextId`(class 共用,記錄「下一個該發的 id 是多少」),還有一個普通成員 `int myId`(每個物件自己的 id)。建構子裡:`myId = nextId; nextId++;`。

```cpp
class IdGenerator {
private:
    static int nextId;
    int myId;
public:
    IdGenerator() {
        // 填這裡
    }
    int getId() {
        return myId;
    }
};

int IdGenerator::nextId = 0;   // class 外初始化,語法規定,先照抄
```

把建構子內容填出來,然後在 main 裡造三個 `IdGenerator`,印出它們的 id(應該是 0、1、2 或 1、2、3,看你 nextId 從幾開始)。試試看。