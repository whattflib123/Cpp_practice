---
title: C++第八章 -- Inheritance繼承 + Virtual

---

# C++第八章 -- Inheritance繼承 + Virtual
> 如果有類似的class，但功能不太一樣

## 1. Inheritance 舉例：
### (a) 舉例: 以下兩種動物，只有叫聲不同
```cpp
class Dog {
public:
    std::string name;
    void eat() { std::cout << name << " is eating\n"; }
    void sleep() { std::cout << name << " is sleeping\n"; }
    void bark() { std::cout << name << " says Woof!\n"; }
};

class Cat {
public:
    std::string name;
    void eat() { std::cout << name << " is eating\n"; }    // 跟 Dog 一模一樣!
    void sleep() { std::cout << name << " is sleeping\n"; } // 跟 Dog 一模一樣!
    void meow() { std::cout << name << " says Meow!\n"; }
};
```
- 解法：用一個基底class，讓其他class 「繼承」
    ```cpp
    class Animal {              // 基底 class(parent / base class)
    public:
        std::string name;
        void eat() { std::cout << name << " is eating\n"; }
        void sleep() { std::cout << name << " is sleeping\n"; }
    };

    class Dog : public Animal {     // Dog「繼承」Animal
    public:
        void bark() { std::cout << name << " says Woof!\n"; }
        //                        ↑ name 是從 Animal 繼承來的,直接能用!
    };

    class Cat : public Animal {     // Cat 也繼承 Animal
    public:
        void meow() { std::cout << name << " says Meow!\n"; }
    };


    // 使用 --------------------------------
    Dog d;
    d.name = "Rex";
    d.eat();      // "Rex is eating"  ← 這是從 Animal 繼承來的,Dog 自己沒寫這個函式
    d.bark();     // "Rex says Woof!" ← 這是 Dog 自己的
    ```

### (b) 建構子與繼承
> 實務上的寫法與解釋，子類別建構子要呼叫父類別建構子
> 因為有可能父類別建構子會需要吃參數

繼承物件 = 父類 + 子類
- Dog 物件 = [Animal 的那一層] + [Dog 自己的那一層]

    ```cpp
    class Animal {
    public:
        std::string name;
        Animal(std::string n) : name(n) {   // 這個建構子「需要」一個字串參數
            std::cout << "Animal 建構: " << name << "\n";
        }
    };

    class Dog : public Animal {
    public:
        std::string breed;

        // ✅ 正確寫法：
        Dog(std::string n, std::string b) 
            : Animal(n),   // 明確告訴編譯器：Animal 那一層要用 n 這個參數來蓋
              breed(b) {   // 再蓋 Dog 自己的部分
            std::cout << "Dog 建構: " << breed << "\n";
        }
    };

    Dog d("Rex", "Labrador");
    // 輸出：
    // Animal 建構: Rex      ← 先蓋 Animal 那層
    // Dog 建構: Labrador    ← 再蓋 Dog 那層
    ```
- 順序:
    - 建構：父類別 -> 子類別
    - 解構：相反

   
    
    
    
## 2. Virtual 
> `virtual`, `override`, `final`
### (a) 舉例
如果 `Animal` 要有一個共同function `makeSound()` 但每種動物的具體作法不同，如何用各自的方式實做，只讓呼叫的人知道如何實做 `makeSound()` 就好

```cpp
class Animal {
public:
    virtual void makeSound() { std::cout << "..."; }   // 基底,前方加 virtual 讓呼叫者知道可以覆寫這個函數
};
class Dog : public Animal {
public:
    void makeSound() override { std::cout << "Woof"; } // 子類覆寫,加 override
};
```
- `virtual`:宣告「子類別可以提供自己的版本覆蓋我」
- `override`:子類別「確實在覆寫」,寫錯名字編譯器會抓

### (b) 多型(polymorphism):
```cpp
Dog d; d.name = "Rex";
Cat c; c.name = "Tom";

Animal& ref1 = d;     // 用「Animal 的參考」指向一個 Dog
Animal& ref2 = c;     // 用「Animal 的參考」指向一個 Cat

ref1.makeSound();     // "Rex says Woof!"  ← 雖然 ref1 的型別寫的是 Animal,
ref2.makeSound();     // "Tom says Meow!"  ←  但實際執行的是 Dog/Cat 自己的版本!
```
→ 呼叫哪個版本由「實際指向的物件」決定,不是參考/指標寫的型別決定

### (c) 統一處理
```cpp 
std::vector<std::unique_ptr<Animal>> animals;
animals.push_back(std::make_unique<Dog>());
animals.push_back(std::make_unique<Cat>());

for (auto& a : animals) {
    a->makeSound();    // 各自跑對的版本,不用 if/else 判斷型別
}
```
- 必須用指標/參考存(`unique_ptr<Animal>`),不能直接 `vector<Animal>`
    - 不然子類自己的部分被砍掉,多型失效
    - Object Slicing

#### (c.1) Object Slicing
```cpp
Dog d;
d.name = "Rex";

Animal a = d;      // 危險！這裡發生「切割」
a.makeSound();      // 呼叫的是 Animal::makeSound()，不是 Dog 的版本！
```
- `Animal a = d;` 不是「d 被裝進一個叫 a 的箱子裡」，而是「照著 Animal 的規格，重新蓋一個全新物件 a，只把 d 裡符合 Animal 規格的那部分資料抄過去」。 Dog 專屬的資料根本沒被抄，所以 Dog 的一切都跟 a 無關。
- 所以之所以用參考，就是因為只是指標指向，而不複製物件本身
    ```cpp
    // 對照組：不會 slicing
    Dog d;
    Animal& a_ref = d;     // 參考，指向同一個 Dog 物件
    a_ref.makeSound();     // 正確呼叫 Dog::makeSound()

    Animal* a_ptr = &d;    // 指標，同理
    a_ptr->makeSound();    // 正確呼叫 Dog::makeSound()
    ```

#### (c.2) `final` 關鍵字
> 用途：鎖死類別或函式，防止之後被誤用繼承/覆寫。

加入關鍵字`final`後，後續類別就不能繼承或覆寫
```cpp
class Dog final : public Animal { /* ... */ };
// class Puppy : public Dog {};   // ❌ Dog 被標記 final，不能再被繼承

class Cat : public Animal {
public:
    void makeSound() final { std::cout << "Meow"; }
    // Cat 可以被繼續繼承，但 makeSound() 不能再被 override
};
```


### (d) 解構子
要確保每個建立起來的物件都會被解構
```cpp
class Animal {
public:
    virtual ~Animal() {}    // 解構子也要 virtual!
};
```


### (e) 純虛函數與抽象類別（Pure Virtual / Abstract Class）
> 「動物」`Animal` 本身是個抽象概念——世界上沒有一隻動物「只是動物」，一定是隻狗、貓、或其他具體物種。這種情況下想強制：
> 1. 不准直接建立 Animal 物件
> 2. 每個繼承 Animal 的子類別都必須自己實作 makeSound()
>
#### (1) 典型語法：
`= 0;` 固定寫法，代表沒有實做，純虛函數 
```cpp
// ===== 抽象基底類別 =====
class Shape {
public:
    virtual ~Shape() {}                          // (1) 解構子，virtual + 有實作

    virtual double area() const = 0;              // (2) 純虛函數 #1
    virtual double perimeter() const = 0;         // (3) 純虛函數 #2

    void printInfo() const {                      // (4) 一般函式，可以有，正常繼承
        std::cout << "Area: " << area() 
                   << ", Perimeter: " << perimeter() << "\n";
        //                ↑ 抽象類別內部也能呼叫純虛函數！
        //                  實際執行哪個版本，由「未來的子類別」決定
    }
};

// ===== 具體子類別，必須實作所有純虛函數 =====
class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}

    double area() const override {                 // (5) 必須 override，簽章要完全一致
        return 3.14159 * radius * radius;
    }
    double perimeter() const override {
        return 2 * 3.14159 * radius;
    }
};

class Rectangle : public Shape {
private:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}

    double area() const override { return width * height; }
    double perimeter() const override { return 2 * (width + height); }
};


// ==================================
// Shape s;                    // ❌ 編譯錯誤，Shape 是抽象類別，不能直接建立物件
Circle c(5.0);                 // ✅ OK，Circle 實作了所有純虛函數
Rectangle r(3.0, 4.0);         // ✅ OK

std::vector<std::unique_ptr<Shape>> shapes;
shapes.push_back(std::make_unique<Circle>(5.0));
shapes.push_back(std::make_unique<Rectangle>(3.0, 4.0));

for (auto& s : shapes) {
    s->printInfo();     // 各自呼叫對的 area()/perimeter()，多型正常運作
}

```

#### (2) 常見應用： 全部函式都是純虛函數 = 純介面（interface）
> 只定義規格，不提供實作

##### a. 規格
```cpp
class InferenceBackend {
public:
    virtual ~InferenceBackend() = default;   // = default 也是常見寫法，效果同 {}
    // 等同於： virtual ~InferenceBackend() {}
    
    // 三個純虛函數 = 規定「每個硬體後端都必須提供這三種能力」
    virtual void loadModel(const std::string& path) = 0;
    virtual std::vector<float> infer(const std::vector<float>& input) = 0;
    virtual std::string backendName() const = 0;
};
```

##### b. 具體硬體後端，各自實做規格
```cpp
// ===== CPU 後端 =====
class CPUBackend : public InferenceBackend {
private:
    std::string modelPath;
    bool modelLoaded = false;

public:
    void loadModel(const std::string& path) override {
        modelPath = path;
        modelLoaded = true;
        std::cout << "[CPU] 載入模型: " << path << "\n";
        // 實際上這裡可能會用 ONNX Runtime CPU provider 之類的邏輯
    }
    
    std::vector<float> infer(const std::vector<float>& input) override {
        std::cout << "[CPU] 用純軟體運算跑推論，輸入大小: " << input.size() << "\n";
        // 假設回傳一個假結果
        return std::vector<float>(input.size(), 0.5f);
    }
    
    std::string backendName() const override {
        return "CPU";
    }
};

// ===== GPU 後端 =====
class GPUBackend : public InferenceBackend {
private:
    std::string modelPath;

public:
    void loadModel(const std::string& path) override {
        modelPath = path;
        std::cout << "[GPU] 透過 CUDA 載入模型: " << path << "\n";
    }
    
    std::vector<float> infer(const std::vector<float>& input) override {
        std::cout << "[GPU] 用 CUDA kernel 跑推論\n";
        return std::vector<float>(input.size(), 0.8f);
    }
    
    std::string backendName() const override {
        return "GPU";
    }
};

// ===== FPGA 後端（貼近你的 Vitis-AI 背景）=====
class FPGABackend : public InferenceBackend {
private:
    std::string bitstreamPath;

public:
    void loadModel(const std::string& path) override {
        bitstreamPath = path;
        std::cout << "[FPGA] 透過 Vitis-AI 載入 bitstream: " << path << "\n";
        // 實際上這裡會呼叫 Vitis-AI runtime API 去配置 DPU
    }
    
    std::vector<float> infer(const std::vector<float>& input) override {
        std::cout << "[FPGA] 送資料進 DPU 做推論\n";
        return std::vector<float>(input.size(), 0.9f);
    }
    
    std::string backendName() const override {
        return "FPGA";
    }
};
```

##### c. 使用時
> 用一個 class 統一管理，就不用使用if/else 的寫法

```cpp
class EdgeInferencePool {
private:
    std::vector<std::unique_ptr<InferenceBackend>> backends;

public:
    void addBackend(std::unique_ptr<InferenceBackend> backend) {
        backends.push_back(std::move(backend));
    }
    
    void runAll(const std::string& modelPath, const std::vector<float>& input) {
        for (auto& backend : backends) {
            std::cout << "=== 使用後端: " << backend->backendName() << " ===\n";
            backend->loadModel(modelPath);
            auto result = backend->infer(input);
            std::cout << "推論完成，輸出大小: " << result.size() << "\n\n";
        }
    }
};

// 使用 =================================
int main() {
    EdgeInferencePool pool;
    
    pool.addBackend(std::make_unique<CPUBackend>());
    pool.addBackend(std::make_unique<GPUBackend>());
    pool.addBackend(std::make_unique<FPGABackend>());
    
    std::vector<float> dummyInput = {1.0f, 2.0f, 3.0f};
    pool.runAll("resnet18.onnx", dummyInput);
    
    return 0;
}

// ❌ 反面教材 ===========================
void runInference(std::string backendType, std::string modelPath) {
    if (backendType == "CPU") {
        // CPU 專屬邏輯...
    } else if (backendType == "GPU") {
        // GPU 專屬邏輯...
    } else if (backendType == "FPGA") {
        // FPGA 專屬邏輯...
    }
    // 每加一種新硬體，這裡就要多一個 else if，到處都要改
}
```

##### 結論
透過使用抽象界面，`runAll()`完全不用管後面有幾種硬體、各自如何運作，只要符合`InferenceBackend` 規格就可以丟進`pool`跑
即便未來新增新的一種硬體，`EdgeInferencePool` 也不用改