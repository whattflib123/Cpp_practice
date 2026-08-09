#include <iostream>

class Box {
    // 自己寫:private 成員、建構子、解構子、set、get
    private:
        int* data;
    public:
        Box(int value){
            data = new int(value);
        }
        ~Box(){
            if(data != nullptr){
                delete data;
                std::cout << "box gone";
            }
        }
        void set(int value){
            *data = value;
        }
        int get(){
            return *data;
        }

};

int main() {
    Box b(10);
    std::cout << b.get() << std::endl;
    b.set(99);
    std::cout << b.get() << std::endl;
    return 0;
}