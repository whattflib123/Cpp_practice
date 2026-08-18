#include <iostream>
#include <algorithm>  // std::copy

class Frame {
    char* buf;
    int   size;
    int   id;
public:
    // TODO 4-1: ctor — new char[size], 印 "alloc #<id>"
    Frame(int id, int size) : buf(nullptr), size(size), id(id){
        buf = new char[size];
        std::cout << "alloc #" << id << '\n';
    }
    // TODO 4-1: dtor — delete[] buf, 印 "free  #<id>"
    ~Frame(){
        std::cout << "free  #" << id << '\n';
        delete[] buf;
    }
    // (先不寫 copy ctor，讓 ASan 抓 double free)
    // aka 沒寫 Frame(const Frame& other)

    // 題 4-2: Frame class 裡加 copy ctor
    Frame(const Frame& other){
        std::cout << "copy  #" << other.id << '\n';
        buf = new char[other.size];
        std::copy(other.buf, other.buf+other.size, buf);
        size = other.size;
        id = other.id;
    }

    // 題 4-3: copy assignment
    Frame& operator=(const Frame& other) { 
        if (this == &other) return *this; // 防呆，防止a = a;這種寫法
        delete[] buf;
        buf = new char[other.size];
        std::copy(other.buf, other.buf+other.size, buf);
        size = other.size;
        id = other.id;
        std::cout << "copy= #" << id << '\n';
        return *this;
    }

    // 題 4-4 move ctor
    Frame (Frame&& other) noexcept:
    buf(other.buf),
    size(other.size),
    id(other.id){
        std::cout << "move  #" << id << '\n';
        other.buf = nullptr; // 清空要加很重要
        other.id = 0;
        other.size = 0;
    }

    // 題 4-5：move assignment
    Frame& operator=(Frame&& other) noexcept{
        
        // 1. 方呆
        if (this == &other) return *this;
        // 2. 清空自己
        delete[] buf; 
        // 3. 接管來源
        buf = other.buf; 
        size = other.size; 
        id = other.id;
        // 4. 清空來源
        other.buf = nullptr; 
        other.size = 0; 
        other.id = 0;

        std::cout << "move  #" << id << '\n';

        return *this;

    }
};

int main() {
    // 題 4-1: 建兩個 Frame，讓 shallow copy 觸發 double free
    Frame a(1, 64);
    // Frame b = a;

    // 題 4-3:「b 已存在，再用 = 覆寫」
    // Frame b(2, 64);   // b 已有自己的 buf
    // b = a;            // copy assignment，不是 copy ctor

    // 題 4-4 move ctor
    // Frame b = std::move(a);

    // 題 4-5：move assignment
    Frame b(2, 64);   // b 已有自己的 buf
    b = std::move(a);

    return 0;
}
