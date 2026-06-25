#include <iostream>

// 把這個改成 class template（能裝任何型別 T）
template <typename T>
class Buffer {
private:
    T* data;
    int size;
public:
    Buffer(int s) {
        size = s;
        data = new T[s]();
    }
    ~Buffer() {
        if (data != nullptr) delete[] data;
    }
    T& operator[](int i) {
        return data[i];
    }
    int getSize() {        // 回傳長度
        return size;
    }
};

int main() {
    Buffer<int> a(3);          // 裝 int
    a[0] = 100;
    std::cout << a[0] << std::endl;        // 100

    Buffer<double> b(3);       // 裝 double
    b[0] = 3.14;
    std::cout << b[0] << std::endl;        // 3.14

    return 0;
}