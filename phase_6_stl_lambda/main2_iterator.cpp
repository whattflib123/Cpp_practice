#include <iostream>
#include <vector>


int main(){
    std::vector<int> v = {3, 7, 1, 9, 4};
    int sum = 0;


    for(auto it = v.begin(); it != v.end(); ++it){
        sum = sum + *it;
        std::cout << *it << std::endl;
    }
    std::cout << sum << std::endl;

    return 0;
}