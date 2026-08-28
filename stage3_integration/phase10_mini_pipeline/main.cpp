#include <iostream>
#include <cstdlib>   // malloc, free

class DmaBuffer {
    void* ptr;
    size_t size;
public:
    explicit DmaBuffer(size_t n)
        : ptr(malloc(n)), size(n) {
        // TODO: 印 "[DmaBuffer] alloc N bytes"
        std::cout << "[DmaBuffer] alloc " << n << " bytes\n";
    }

    ~DmaBuffer() {
        // TODO: 印 "[DmaBuffer] free"
        std::cout << "[DmaBuffer] free\n";
        free(ptr);
    }

    // copy 建構子
    DmaBuffer(const DmaBuffer&) = delete;
    DmaBuffer& operator=(const DmaBuffer&) = delete;

    DmaBuffer(DmaBuffer&& o) noexcept
        : ptr(o.ptr), size(o.size) {
        o.ptr = nullptr;
        o.size = 0;
        // TODO: 印 "[DmaBuffer] move"
        std::cout << "[DmaBuffer] move\n";

    }

    DmaBuffer& operator=(DmaBuffer&& o) noexcept {
        if (this != &o) {
            free(ptr);
            ptr = o.ptr; size = o.size;
            o.ptr = nullptr; o.size = 0;
        }
        return *this;
    }

    void* data() const { return ptr; }
    size_t bytes() const { return size; }
};


class Frame {
    DmaBuffer buf;
    int id;                        
public:
    Frame(int id, size_t size)
        : buf(size), id(id) {
        // 印 "[Frame N] created"
        std::cout << "[Frame " << id << "] created\n";
    }

    // copy 禁止（DmaBuffer 已 delete copy，Frame 自動繼承）
    // copy 建構子 不用寫，自動繼承
    // Frame(const Frame&) = delete;
    // Frame& operator=(const Frame&) = delete;

    // move：compiler 自動產生即可（為什麼？）
    // destructor：compiler 自動產生即可（為什麼？）

    int get_id() const { return id; }
    DmaBuffer& buffer() { return buf; }
};




int main() {
    // TODO: 建一個 DmaBuffer(1228800)，move 給另一個，觀察 log 順序
    Frame a(0, 1228800);
    Frame b = std::move(a);
    std::cout << "b id=" << b.get_id() << '\n';

    }
