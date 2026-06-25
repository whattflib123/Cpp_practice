#include <iostream>

class Buffer {
private:
    int* data;
    int size;
public:
    Buffer(int s) {
        size = s;
        data = new int[s]();
    }
    ~Buffer() {
        if (data != nullptr) {
            delete[] data;
        }
    }

    // TODO: 加一個 operator[],回傳第 i 格的「參考」

    int& operator[](int i){
        return data[i];
    }
};

int main() {
    Buffer buf(5);
    buf[0] = 100;              // 用 [] 寫入
    buf[4] = 999;
    std::cout << buf[0] << std::endl;   // 100
    std::cout << buf[4] << std::endl;   // 999
    return 0;
}