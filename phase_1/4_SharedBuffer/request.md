題 3:SharedBuffer —— 自己實作引用計數
情境
你有一塊很大的資料(想像是模型權重),好幾個地方要同時讀它。誰都不該獨佔,但也不能有人用完就釋放害到別人。解法:共享所有權 + 計數。最後一個使用者離開時,才釋放。
兩塊共享的東西
每個 SharedBuffer 物件內部存兩根指標,都指向 heap 上的共享資源:

int* data:共享的資料本身
int* ref_count:共享的計數器(放 heap 才能讓所有 copy 共用同一個)

規格與行為

建構子:配置 data、配置 ref_count 並設為 1(第一個主人)。
Copy 建構子:有人複製我時——data 和 ref_count 指標都抄過去(指向同一塊),然後 計數 +1。
解構子:計數 -1;如果減完變 0,才真正 delete data 和 ref_count,並印 "buffer freed";不為 0 就只印 "ref count now: X"。
count():回傳目前計數(方便觀察)。

要能跑出這樣
```cpp
int main() {
    SharedBuffer a(42);              // 計數 = 1
    std::cout << a.count() << "\n";  // 1
    {
        SharedBuffer b = a;          // copy!計數 = 2
        std::cout << a.count() << "\n";  // 2
    }   // b 離開 scope → 計數 -1 → 變 1(不釋放,a 還在用)→ 印 "ref count now: 1"

    std::cout << a.count() << "\n";  // 1
    return 0;
}   // a 離開 → 計數 -1 → 變 0 → 釋放 → 印 "buffer freed"
```
預期輸出:
1
2
ref count now: 1
1
buffer freed