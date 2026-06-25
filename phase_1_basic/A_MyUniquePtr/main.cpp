#include <iostream>

class MyUniquePtr {
private:
    int* ptr;   // 指向 heap 上那塊 int

public:
    // 建構子
    MyUniquePtr(int value) {
        // TODO 1: 在 heap 配置一個 int,值是 value,存進 ptr
        ptr = new int(value); 
        // 等效於：
        // ptr = new int; // 在記憶體中配置一個int，存在指標變數ptr中，此時ptr裡面存的是address
        // *ptr = value;  // 將value的值，存進ptr所指的記憶體

        // 上面那句的意思是 配置(new)一塊記憶體 給 變數(value) 型態是int
        // why?
        // 因為ptr 是指標 int* ，所以要讓它指向一塊記憶體;，並在裡面放入value 
    } 

    // 解構子
    ~MyUniquePtr() {
        // TODO 2: 釋放 ptr,並印出 "freed"
        //         (提示:被 move 過的物件 ptr 是 nullptr,delete nullptr 是安全的,
        //          但 "freed" 要不要印你自己想想)
        if (ptr != nullptr){
            delete ptr;
            std::cout << "freed" << std::endl;
        }


    }

    // 解參考:回傳「參考」才能讓 *p = 100 改到值
    // 這是一個 class 裡面的 function
    int& operator*() {  
    // 回傳型別是int& (回傳一個int的參考) ; 
    // operator*()是 operator overloading，當有人對這個類別的物件使用運算子時，要執行這個函數
    // C++ 允許在class中定義運算子寫函式
    // 語法 operator運算子
    // 例如： operator+, operator-, operatpr==, ...

        // TODO 3: 回傳 ptr 指向的值
        return *ptr：
    }

    // ↓↓↓ 規格 4、5 的語法先給你,看得懂就好,之後會專門教 ↓↓↓

    // 禁止複製(這兩行就是「禁 copy」的標準寫法)
    MyUniquePtr(const MyUniquePtr&) = delete;
    MyUniquePtr& operator=(const MyUniquePtr&) = delete;

    // 支援 move:過戶建構子
    MyUniquePtr(MyUniquePtr&& other) {
        ptr = other.ptr;        // 把對方的指標抄過來
        other.ptr = nullptr;    // 對方設空 → 這就是「過戶後原物件變空殼」
    }
};

int main() {
    MyUniquePtr p(42);
    std::cout << *p << std::endl;        // 42
    *p = 100;
    std::cout << *p << std::endl;        // 100

    MyUniquePtr q = std::move(p);        // 過戶給 q
    std::cout << *q << std::endl;        // 100

    return 0;
}   // 這裡應該印出 "freed"(q 解構釋放;p 是空殼,無資源可放)