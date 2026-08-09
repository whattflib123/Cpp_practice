# include <iostream>

int main(){
    std::cout << "__cplusplus = " << __cplusplus << '\n';
    
    int* p = new int[3];

    p[0] = 0;
    p[1] = 1;
    p[2] = 2;

    for (int i = 0; i < 3; i++){
        std::cout << p[i] << std::endl;
    }

    p[3] = 99;
    delete[] p;

}