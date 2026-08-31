#include <iostream>
#include <memory>
#include <vector>
#include <span>

class Animal {
public:
    virtual void speak(){
        std::cout << "Animal speak\n";
    };   // 印 "Animal speak"
};

class Dog : public Animal {
public:
    void speak(){
        std::cout << "Dog: Woof\n";
    };   // 印 "Dog: Woof"
};

class Cat : public Animal {
public:
    void speak(){
        std::cout << "Cat: Meow\n";
    };   // 印 "Cat: Meow"
};



// 12-2 
struct NoVirtual {
    int x;
    void speak();
};
                                    
struct WithVirtual {
    int x;
    virtual void speak();
};



// 12-3
// class InferenceBackend {
// public:
//     virtual void run() = 0;   // pure virtual
// };

// class TrtBackend : public InferenceBackend {
// public:
//     void run() override { std::cout << "TrtBackend::run\n"; }
// };

// class CpuBackend : public InferenceBackend {
// public:
//     void run() override { std::cout << "CpuBackend::run\n"; }
// };




// 12-4
class Base {                       
public:
    virtual ~Base() { std::cout << "Base::~Base\n"; }
};

class Derived : public Base {
public:
    std::unique_ptr<int[]> buf;
    Derived() : buf(std::make_unique<int[]>(1024)) {
        std::cout << "Derived::Derived, buf allocated\n";
    }
    ~Derived() { std::cout << "Derived::~Derived\n"; }
};


// 12-5
class Sensor {
public:
    virtual void read() { std::cout << "Sensor::read\n"; }
    virtual ~Sensor() = default;
};

class LidarSensor : public Sensor {
public:
    void Read() { std::cout << "LidarSensor::Read\n"; }  // 故意大寫 R
};


// 12-6
class InferenceBackend {
public:
    virtual void run(std::span<float> data) = 0;
    virtual ~InferenceBackend() = default;
};
class TrtBackend2 : public InferenceBackend {
public:
    void run(std::span<float> data) override {
        std::cout << "TrtBackend::run, size=" << data.size() << "\n";
    }

};

class CpuBackend2 : public InferenceBackend {
public:
    void run(std::span<float> data) override {
        std::cout << "CpuBackend::run, size=" << data.size() << "\n";
    }
};




int main(){

    Dog d;
    Cat c;
    Animal* p1 = &d;
    Animal* p2 = &c;
    p1->speak();
    p2->speak();

    // 沒家 virtual
    // Animal speak
    // Animal speak

    // 加了 virtual 
    // Dog: Woof
    // Cat: Meow



    // -----------------------------------
    // 12-2 比較 with virtual 的 size 大小
    // -----------------------------------
    std::cout << "NoVirtual:   " << sizeof(NoVirtual)  << "\n";
    std::cout << "WithVirtual: " << sizeof(WithVirtual) << "\n";




    // -----------------------------------
    // 12-3 
    // -----------------------------------
    // 用 InferenceBackend* 呼叫，不知道底層是哪個
    // TrtBackend trt;
    // CpuBackend cpu;
    // InferenceBackend* backends[] = {&trt, &cpu};
    // for (auto* b : backends) b->run();

    // InferenceBackend b;


    // -----------------------------------
    // 12-4 virtual dtor
    // -----------------------------------
    Base* p = new Derived();
    delete p;

    // -----------------------------------
    // 12-5 override 標記這個函式是覆寫，編譯器強制檢查
    // -----------------------------------
    // LidarSensor lidar;
    // Sensor* s = &lidar;
    // s->read();


    // -----------------------------------
    // 12-6：unique_ptr<Base> 整合
    // -----------------------------------
    std::vector<float> buf(16, 1.0f);

    std::unique_ptr<InferenceBackend> backend;
    bool use_trt = false;   // 改成 false 換成 CpuBackend2

    if (use_trt)
    backend = std::make_unique<TrtBackend2>();
    else
    backend = std::make_unique<CpuBackend2>();

    backend->run(buf);

    
}