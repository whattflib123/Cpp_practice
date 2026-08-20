#include <iostream>
#include <span>
#include <vector>


// 版本 A — 傳統裸指標
void print_raw(const int* data, int len) {
    // 印 len，再印所有元素
    printf("%d\n", len);
    for (int i = 0; i <  len; i++)
    {
        /* code */
        std::cout << data[i] << '\n';
    }
    
}

// 版本 B — std::span
void print_span(std::span<const int> s) {
    // 印 s.size()，再印所有元素
    std::cout << s.size() << '\n'; // 沒傳長度進去，但span卻可以取得長度資訊
    for (size_t i = 0; i < s.size(); i++)
    {
        /* code */
        std::cout << s[i] << '\n';
    } 
}

std::span<int> make_span() {
    std::vector<int> v = {1, 2, 3};
    return std::span<int>(v);
}



int main() {
    int arr[5] = {10, 20, 30, 40, 50};
    print_raw(arr, 5);
    print_span(arr);   // 不需要傳長度

    std::vector<int> vec = {1, 2, 3, 4, 5};
    // std::span<int> s = vec;
    std::span<const int> s = vec;

    std::cout << "vec.data() = " << vec.data() << '\n';
    std::cout << "s.data()   = " << s.data()   << '\n';

    //s[0] = 999;
    std::cout << "vec[0] after s[0]=999: " << vec[0] << '\n';


    // auto s2 = make_span();
    // std::cout << s2[0] << '\n';  // dangling


    std::vector<float> output(25);

    // 先填假資料
    for (int i = 0; i < 5; i++) {
        output[i*5+0] = i * 10.0f;   // x
        output[i*5+1] = i * 10.0f;   // y
        output[i*5+2] = 50.0f;       // w
        output[i*5+3] = 30.0f;       // h
        output[i*5+4] = 0.5f + i * 0.1f;  // score
    }

    std::span<float> all = output;
    for (int i = 0; i < 5; i++) {
        auto box = all.subspan(i*5, 5);
        std::cout << "box " << i << ": score = " << box[4] << '\n';
    }

}