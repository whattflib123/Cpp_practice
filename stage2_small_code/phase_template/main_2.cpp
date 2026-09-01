#include <iostream>
#include <typeinfo>





struct Frame {
    int id;
    std::string name;
    Frame(int i, std::string n) : id(i), name(std::move(n)) {
        std::cout << "Frame ctor: " << id << " " << name << "\n";
    }
};

template <typename T, typename... Args>
T make_obj(Args&&... args) {
    return T(std::forward<Args>(args)...);
}

int main() {
    auto f = make_obj<Frame>(1, "cam0");
}