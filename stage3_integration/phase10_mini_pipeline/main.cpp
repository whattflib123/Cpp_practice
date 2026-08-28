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

int main() {
    // TODO: 建一個 DmaBuffer(1228800)，move 給另一個，觀察 log 順序
    DmaBuffer a(1228800);
    DmaBuffer b = std::move(a);
}
