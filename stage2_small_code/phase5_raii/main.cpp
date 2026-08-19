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
    if (fail) return;
    // TODO: free(ptr)
    // free(ptr);
}


// 題 5-3: 改用 MallocBuffer，throw 也不 leak
// void process_raii(bool fail) { ... }

int main() {
    // 題 5-1
    process_raw(true);   // leak
    process_raw(false);  // 正常

    // 題 5-2 之後換這個
    // process_raii(true);
    // process_raii(false);

    return 0;
}
