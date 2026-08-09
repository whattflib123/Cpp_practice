#include <iostream>

class IntStack{

    private:
        int* data;
        int capacity;
        int count;

    public:
        IntStack(int capacity){
            data = new int[capacity]();
            count = 0;
        }
        ~IntStack(){
            if (data != nullptr){
                delete[] data;
                std::cout << "stack freed" << std::endl;
            }
        }
        void push(int value){
            data[count] = value;
            count = count + 1;
        }
        int top(){
            return data[count-1];
        }
        int getSize(){
            return count;
        }



};


int main() {
    IntStack s(10);          // 容量 10 的 stack
    s.push(100);
    s.push(200);
    s.push(300);
    std::cout << s.top() << std::endl;       // 300(最後放的)
    std::cout << s.getSize() << std::endl;   // 3
    return 0;
}   // 印出 "stack freed"