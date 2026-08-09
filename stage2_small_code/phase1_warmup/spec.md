/*
規格

phase1/main.cpp:

1. 結構
struct Detection {
    int   id;
    float score;
    float x, y, w, h;
};

2. 資料
std::vector<Detection> 塞 5 筆,score 高低都有(例如 0.95 / 0.42 / 0.88 / 0.31 / 0.67)。用 brace init。

3. 核心觀察 —— 三種迴圈

依序跑,每圈印出元素位址:
for (auto d : v)          { std::cout << &d << '\n'; }
for (const auto& d : v)   { std::cout << &d << '\n'; }
for (auto& d : v)         { std::cout << &d << '\n'; }
另外先印一次 &v[0]、&v[1] 當基準。

看什麼:
- 哪幾圈的位址跟 &v[i] 一致?
- by value 那圈的位址有什麼特徵?為什麼 5 次迭代印出來的位址是同一個?

這題答得出來,「for range 要不要加 &」就不再是背的。

4. 兩支函式
int count_above(const std::vector<Detection>& v, float thr);
std::vector<Detection> filter(const std::vector<Detection>& v, float thr);
count_above 數 score > thr 的筆數。filter 回傳篩選後的新 vector。

5. 排版輸出
void print(const Detection& d);
用 std::fixed + std::setprecision(2)(要 #include <iomanip>),讓 score 印成 0.95 而不是 0.949999988。

順手觀察:直接 std::cout << 0.95f 印出什麼?為什麼?

6. main 流程
印全部 → 印 count_above(v, 0.5f) → filter(v, 0.5f) 印篩選結果。

目標 60~80 行。

骨架

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

int main() {
    std::vector<Detection> v = {
        // TODO: 5 筆
    };

    // TODO
}

編譯照舊:
g++ -std=c++20 -Wall -Wextra -g -fsanitize=address main.cpp -o main
./main

寫完要能答

1. 三種迴圈位址差在哪?auto 為什麼不會自己帶走 &?(推導規則,面試高頻)
2. filter 回傳整個 vector,會複製一份嗎?(考點:RVO / NRVO / move —— Stage 3 主餐,先建立直覺)
3. const std::vector<Detection>& 裡的 const 和 & 各自擋掉什麼?拿掉其中一個分別會怎樣?
4. Detection 是 24 bytes。by value 傳有問題嗎?多大才該改傳 reference?小型 POD by value 反而可能比較快,為什麼?
*/