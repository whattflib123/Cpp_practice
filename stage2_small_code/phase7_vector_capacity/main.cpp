#include <iostream>
#include <vector>

int main() {
    // 7-1 7-2
    // std::vector<int> v;
    // v.reserve(20);
    // for (int i = 0; i < 20; i++) {
    //     v.push_back(i);
    //     std::cout << "size=" << v.size()
    //             << " cap=" << v.capacity() << '\n';
    // }

    // 7-3 
    // std::vector<int> v = {1, 2, 3}; // 配置12bytes
    // auto it = v.begin(); // it 指向v.begin那塊記憶體
    // v.push_back(4); // 再次塞一個，發現記憶體不夠，re-allocation更大塊的，元素班過去，舊的free
    // std::cout << *it << '\n'; // 讀取已經釋放的 -> 報錯：AddressSanitizer: heap-use-after-free 

    // 7-4
    std::vector<int> v = {1, 2, 3}; // 配置12bytes
    v.reserve(4); // 預先分配記憶體

    auto it = v.begin(); // it 指向v.begin那塊記憶體
    v.push_back(4); 
    std::cout << *it << '\n';  


}
