#include <iostream>
#include <typeinfo>


// 13-1 basic
template <typename T>
T my_max(T a, T b) {
    T result = (a > b) ? a : b;
    std::cout << "T=" << typeid(T).name() << " result=" << result << "\n";
    return result;
}


// 13-2
template <typename T, typename U>
class Pair {
public:
    // 存 first（型別 T）和 second（型別 U）
    // 提供 ctor 和 print() method
    T first;
    U second;

    Pair(T f, U s) : first(f), second(s){

    }

   void print(){
        std::cout << "first: " << first << '\n';
        std::cout << "second: " << second << '\n';
    }
};

// 13-3：型別推導實驗

template <typename T>
void show_type(T x) {
    std::cout << typeid(T).name() << "\n";
}



// 13-4:
// A：rvalue reference（Type 固定）
void foo(int&& x) {
    std::cout << "rvalue ref: " << x << "\n";
}

// B：forwarding reference（T 是 template 參數）
template <typename T>
void bar(T&& x) {
    std::cout << "forwarding ref\n";
}


// 13-5 
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




int main(){

    // 13-1
    // my_max(3, 5);       // int
    // my_max(1.2, 0.8);   // double
    // my_max('a', 'z');   // char


    // 13-2
    Pair<int, std::string> p1(42, "hello");
    Pair<float, char> p2(3.14f, 'x');
    p1.print();
    p2.print();
    
    // 13-3 
    int a = 1;
    const int b = 2;
    int& r = a;

    show_type(a);
    show_type(b);
    show_type(r);

    // 13-4
    {
        int a_4 = 5;
        // foo(a_4);       // 能不能編譯？
        foo(5);          // 能不能編譯？
        bar(a_4);          // 能不能編譯？
        bar(5);          // 能不能編譯？

    }

    // 13-5
    int a_5 = 10;
    wrapper_bad(a_5);    // 傳 lvalue
    wrapper_bad(42);   // 傳 rvalue，但 wrapper_bad 裡變成 lvalue

    wrapper_good(a_5);   // 傳 lvalue
    wrapper_good(42);  // 傳 rvalue，wrapper_good 保留 rvalue
}