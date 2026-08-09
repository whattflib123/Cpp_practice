#include <iostream>
#include <vector>
#include <algorithm>

int main(){

    std::vector<int> v = {5, 2, 8, 1, 9};

    std::sort(v.begin(), v.end());

    for (int x : v){
        std::cout << x << std::endl;
    }
    return 0; 
}