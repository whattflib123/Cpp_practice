#include <iostream>

class Buffer{
    private:
        int* data;
        int size;

    public:
        Buffer(int value){
            size = value;
            data = new int[size]();
        }
        ~Buffer(){
            if(data != nullptr){
                std::cout << "freed" << std::endl;
                delete[] data;
            }
        }

        // copy
        Buffer(const Buffer& other){
            size = other.size;
            data = new int[size]();
            for(int i = 0; i < size; i++){
                data[i] = other.data[i];
            }  
        }
        
        // move
        Buffer(Buffer&& other){
            data = other.data;
            size = other.size;
            other.data = nullptr;
        }

        void set(int i, int value){
            data[i] = value;
        }

        int get(int i){
            return data[i];
        }


};




int main() {
    Buffer a(3);
    a.set(0, 100);

    Buffer b = a;              // copy(深複製):b 是獨立的一塊
    b.set(0, 999);             // 改 b 不該影響 a

    std::cout << a.get(0) << std::endl;   // 100(沒被 b 影響 → 證明是深複製)
    std::cout << b.get(0) << std::endl;   // 999

    Buffer c = std::move(a);   // move:c 接管 a 的資源,a 變空殼
    std::cout << c.get(0) << std::endl;   // 100

    return 0;
}