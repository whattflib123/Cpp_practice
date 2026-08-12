#include <iostream>
#include <iomanip>
#include <vector>

struct Detection {
    int   id;
    float score;
    float x, y, w, h;
};

void print(const Detection& d);
int  count_above(const std::vector<Detection>& v, float thr);
std::vector<Detection> filter(const std::vector<Detection>& v, float thr);



// ===============================================================
// 第 4. count_above 數 score > thr 的筆數。filter 回傳篩選後的新 vector。
// ===============================================================

int count_above(const std::vector<Detection>& v, float thr){
    int n = 0;
    for(const auto& d : v){
        if(d.score > thr){
            n = n + 1;
        }
    }
    return n;
} ; 

std::vector<Detection> filter(const std::vector<Detection>& v, float thr){
    std::vector<Detection> filtered;
    for(const auto& d : v){
        if(d.score > thr){
            filtered.push_back(d);
        }
    }
    return filtered;  
};

void print(const Detection& d){
    std::cout << std::fixed << std::setprecision(2) << d.score << std::endl;
}


int main(){

    int num = 0;
    
    std::vector<Detection> v = {
        {0, 0.42f, 0.01f, 0.01f, 0.01f, 0.01f},
        {1, 0.88f, 0.01f, 0.01f, 0.01f, 0.01f},
        {2, 0.31f, 0.01f, 0.01f, 0.01f, 0.01f},
        {3, 0.67f, 0.01f, 0.01f, 0.01f, 0.01f},
        {4, 0.95f, 0.01f, 0.01f, 0.01f, 0.01f},
    };

    // ===============================================================
    // 第 1. ~ 3. 
    // ===============================================================
    std::cout << "=========== 第 1. ~ 3. ================" << '\n';

    std::cout << &v[0] << '\n';
    for (auto d : v)          { 
        std::cout << &d << '\n'; 
    }
    /*
    印出:
    0x50c000000040 基準
    0x7d0f8d000140
    0x7d0f8d000140
    0x7d0f8d000140
    0x7d0f8d000140
    0x7d0f8d000140

    五次的記憶體位置都一樣

    代表:
    1. auto d 把 v 的資料複製並重複用在同一塊記憶體上: 0x7d0f8d000140
    因此 d 不是真正的 v[i] ，這次迴圈重複了五次 建構 + 解構
    v 是 std::vector<>
    d 是 Decection

    2. auto 並不會帶走參考 & ，只留下 "值" ，要 reference 自己寫出來

    */


    std::cout << '\n';
    std::cout << &v[0] << '\n';
    for (const auto& d : v){ 
        std::cout << &d << '\n'; 
    }
    std::cout << '\n';
    std::cout << &v[0] << '\n';
    for (auto& d : v) {
         std::cout << &d << '\n'; 
    }


    /*
    印出:
    0x50c000000040 基準
    0x50c000000040
    0x50c000000058
    0x50c000000070
    0x50c000000088
    0x50c0000000a0

    0x50c000000040 基準
    0x50c000000040
    0x50c000000058
    0x50c000000070
    0x50c000000088
    0x50c0000000a0
    
    1. 代表後面兩種寫法是真的拿 d 來用，而不是真的建構解構
    2. auto& d       // d 是 Detection&       —— 綁定,可改
       const auto& d // d 是 const Detection& —— 綁定,唯讀
    */

    std::cout <<  0.95f << '\n';
 


    num = count_above(v, 0.5f);
    std::cout << num << std::endl;

    std::vector<Detection> filterd = filter(v, 0.5f);
    for (const auto& d : filterd){
        print(d);
    }


}



