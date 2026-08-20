#include <iostream>
#include <cstdlib>    // malloc, free
#include <stdexcept>  // std::runtime_error

// 題 5-2: RAII wrapper
// class MallocBuffer { ... };
class MallocBuffer
{
    void* ptr;
    size_t size;

public:
    MallocBuffer(size_t size):ptr(nullptr), size(size){
        ptr = malloc(size);
        std::cout << "malloc " << size << " bytes" << '\n';
    }
    // copy
    MallocBuffer(const MallocBuffer&)            = delete;
    MallocBuffer& operator=(const MallocBuffer&) = delete;
    // move
    MallocBuffer(MallocBuffer&& other) noexcept :ptr(other.ptr), size(other.size) { 
        // ptr = other.ptr;
        // size = other.size;

        other.ptr = nullptr;
        other.size = 0;
        std::cout << "move " << size << " bytes" << '\n';
    }
    ~MallocBuffer(){
        free(ptr);
        std::cout << "free " << size << " bytes" << '\n';
    }
};



// 題 5-1: 裸 malloc/free，提早 return 造成 leak
void process_raw(bool fail) {
    // TODO: malloc(64)
    // int* ptr = (int*)malloc(64);
    MallocBuffer buf(64);
    // TODO: if (fail) return;  ← 沒 free，leak
    // if (fail) return;
    if (fail) throw std::runtime_error("oops");
    // TODO: free(ptr)
    // free(ptr);
}


// 題 5-3: 改用 MallocBuffer，throw 也不 leak
// void process_raii(bool fail) { ... }

int main() {
    // 題 5-1
    // process_raw(true);   // leak
    // process_raw(false);  // 正常

    // 題 5-2 之後換這個
    // process_raii(true);
    // process_raii(false);

    // try { process_raw(true); }
    // catch (const std::exception& e) {
    //     std::cout << "caught: " << e.what() << '\n';
    // }
    // process_raw(false);

    MallocBuffer a(64);
    MallocBuffer b = std::move(a);   // 應該編譯報錯


    return 0;
}
