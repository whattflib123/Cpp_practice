#include <iostream>



class IdGenerator {
private:
    static int nextId;
    int myId;
public:
    IdGenerator() {
        // 填這裡
        myId = nextId;
        nextId = nextId + 1;
    }
    int getId() {
        return myId;
    }
};



int IdGenerator::nextId = 0;   // class 外初始化,語法規定,先照抄


int main(){
    IdGenerator id1;
    std::cout << id1.getId() << std::endl;

    IdGenerator id2;
    std::cout << id2.getId() << std::endl;

    IdGenerator id3;
    std::cout << id3.getId() << std::endl;
}